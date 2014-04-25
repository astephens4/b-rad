#include <config.h>
#include <circ_buf.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "uart.h"
#include "libLCD.h"
#include "Si4735.h"

extern int currentPreset;
extern int currentBand;

extern struct RSQ g_tuneStatus;

extern union Status lastReceived;

enum OPCODES {
    TUNE = 0,
    TUNE_PRESET,
    VOLUME,
    BAND,
    SET_PRESET,
    NEXT,
    DISPLAY
};

enum DisplayModes {
    STATION_INFO = 0,
    FREQ_AND_TITLE,
    FREQ_AND_ARTIST,
    FREQ_AND_ID,
    PRESETS_FM,
    PRESETS_AM
};

#define convert_freq(band, freq_b) (band==FM) ? ((freq_b)+880)*10 : ((freq_b)+54)*10

int main(void)
{
    init_serial();
    initLCD();
    initRadio();

    unsigned char volume = getVolume();

    // tune this bitch to 102.7
    seek(FM, UP, 1);
    tuneStatus();

    char lcdStr[33];

    putsLCD("Radio!", LCD_SAMELINE);

    enum DisplayModes dispMode;
    int refresh = 0;

    while(1) {
        if(available_serial() >= 6) {
            char current = read_serial();
            if(current != 128) {
                continue;
            }
            refresh = 1;
            // read the 4-bytes length
            long length = (long)read_serial() << 24;
            length |= (long)read_serial() << 16;
            length |= (long)read_serial() << 8;
            length |= (long)read_serial();

            current = read_serial();
            char opcode = (current & 0xE) >> 5;
            switch(opcode) {
                case TUNE:
                {
                    enum Band band = current & 0x10 >> 4;
                    char freq_b = read_serial();
                    int freq = convert_freq(band, freq_b);
                    //tuneStation(band, freq);
                    if(band == FM) {
                        sprintf(lcdStr, "Tune FM: %d", freq);
                    }
                    else {
                        sprintf(lcdStr, "Tune AM: %d", freq);
                    }
                    break;
                }
                case TUNE_PRESET:
                {
                    enum Band band = current & 0x10 >> 4;
                    int preset = current & 0x0F;
                    //tunePreset(band, preset);
                    if(band == FM) {
                        sprintf(lcdStr, "Tune FM Preset: %d", preset);
                    }
                    else {
                        sprintf(lcdStr, "Tune AM Preset: %d", preset);
                    }
                }
                    break;
                case VOLUME:
                {
                    enum Direction dir = current & 0x10 >> 4;
                    if(dir == UP) {
                        ++volume;
                    }
                    else {
                        --volume;
                    }
                    //setVolume(volume);
                    sprintf(lcdStr, "Volume %d", volume);
                }
                    break;
                case BAND:
                {
                    enum Band band = current & 0x10 >> 4;
                    //tuneStation(band, getLastStation(band));
                    if(band == FM) {
                        sprintf(lcdStr, "Band set to FM");
                    }
                    else {
                        sprintf(lcdStr, "Band set to AM");
                    }
                }
                    break;
                case SET_PRESET:
                {
                    enum Band band = current & 0x10 >> 4;
                    int preset = current & 0x0F;
                    char freq_b = read_serial();
                    int freq = convert_freq(band, freq_b);
                    //setPreset(band, preset, freq);
                    if(band == FM) {
                        sprintf(lcdStr, "Set FM Preset %d: %d", preset,  freq);
                    }
                    else {
                        sprintf(lcdStr, "Set AM Preset %d: %d", preset,  freq);
                    }
                    break;
                }
                    break;
                case NEXT:
                {
                    enum Direction dir = current & 0x10 >> 4;
                    enum Mode mode = current & 0x0C >> 2;
                    if(mode == PRESET) {
                        currentPreset += (dir) ? 1 : -1;
                        if(currentPreset < 0) currentPreset = 5;
                        if(currentPreset > 5) currentPreset = 0;
                        //tunePreset(currentBand, currentPreset);
                        sprintf(lcdStr, "Tune preset %d", currentPreset);

                    }
                    else if(mode == FREQUENCY) {
                        int currentFreq = getFrequency();
                        currentFreq += (dir) ? 10 : -10;
                        if(currentBand == FM) {
                            if(currentFreq < FM_MIN_FREQ) currentFreq = FM_MAX_FREQ;
                            if(currentFreq > FM_MAX_FREQ) currentFreq = FM_MIN_FREQ;
                            sprintf(lcdStr, "Tune FM %d", currentFreq);
                        }
                        else {
                            if(currentFreq < AM_MIN_FREQ) currentFreq = AM_MAX_FREQ;
                            if(currentFreq > AM_MAX_FREQ) currentFreq = AM_MIN_FREQ;
                            sprintf(lcdStr, "Tune AM %d", currentFreq);
                        }
                        tuneStation(currentBand, currentFreq);
                    }
                    else if(mode == SCAN) {
                        //seek(currentBand, dir);
                        sprintf(lcdStr, "Seek activated");
                    }
                }
                    break;
                case DISPLAY:
                {
                    dispMode = (current & 0x1C) >> 2;
                    sprintf(lcdStr, "Set displayMode %d", dispMode);
                }
                    break;
            }
        }
        if(refresh) {
            refresh = 0;
            clearLCD();
            putsLCD(lcdStr, LCD_NEXTLINE);
            LCD_DELAY(0xFFFF);
            LCD_DELAY(0xFFFF);
            LCD_DELAY(0xFFFF);
        }
    }
    /*

    while(1) {
        if(dispStat == 0) {
            setCursorLCD(1, 0);
            strncpy(lcdStr, "SNR: ", 6);
            itoa(lcdStr+5, g_tuneStatus.snr, 10);
            putsLCD(lcdStr, LCD_NOWRAP);
        }
        else if(dispStat == 1) {
            setCursorLCD(1, 0);
            strncpy(lcdStr, "RSSI:", 6);
            itoa(lcdStr+5, g_tuneStatus.rssi, 10);
            putsLCD(lcdStr, LCD_NOWRAP);
        }
        else if(dispStat == 2) {
            setCursorLCD(1, 0);
            strncpy(lcdStr, "MLTI:", 6);
            itoa(lcdStr+5, g_tuneStatus.multi, 10);
            putsLCD(lcdStr, LCD_NOWRAP);
        }
        else if(dispStat == 3) {
            setCursorLCD(1, 0);
            strncpy(lcdStr, "FREQ:", 6);
            itoa(lcdStr+5, g_tuneStatus.intFlags, 10);
            putsLCD(lcdStr, LCD_NOWRAP);
        }
        else if(dispStat == 4) {
            setCursorLCD(1, 0);
            strncpy(lcdStr, "CTS: ", 6);
            if(lastReceived.radio.CTS) {
                itoa(lcdStr+5, 1, 10);
            }
            else {
                itoa(lcdStr+5, 0, 10);
            }
            putsLCD(lcdStr, LCD_NOWRAP);
        }
        else if(dispStat == 5) {
            setCursorLCD(1, 0);
            strncpy(lcdStr, "ERR :", 6);
            if(lastReceived.radio.ERR) {
                itoa(lcdStr+5, 1, 10);
            }
            else {
                itoa(lcdStr+5, 0, 10);
            }
            putsLCD(lcdStr, LCD_NOWRAP);
        }
        else if(dispStat == 6) {
            setCursorLCD(1, 0);
            strncpy(lcdStr, "RDSI:", 6);
            if(lastReceived.radio.RDSINT) {
                itoa(lcdStr+5, 1, 10);
            }
            else {
                itoa(lcdStr+5, 0, 10);
            }
            putsLCD(lcdStr, LCD_NOWRAP);
        }
        else if(dispStat == 7) {
            setCursorLCD(1, 0);
            strncpy(lcdStr, "RSQI:", 6);
            if(lastReceived.radio.RSQINT) {
                itoa(lcdStr+5, 1, 10);
            }
            else {
                itoa(lcdStr+5, 0, 10);
            }
            putsLCD(lcdStr, LCD_NOWRAP);
        }
        else if(dispStat == 8) {
            setCursorLCD(1, 0);
            strncpy(lcdStr, "STCI:", 6);
            if(lastReceived.radio.STCINT) {
                itoa(lcdStr+5, 1, 10);
            }
            else {
                itoa(lcdStr+5, 0, 10);
            }
            putsLCD(lcdStr, LCD_NOWRAP);
        }
        dispStat = (dispStat+1)%9;
        if(dispStat == 0) {
            tuneStatus();
            getStatus();
            __asm("nop");
        }
        LCD_DELAY(0xFFFF);
        LCD_DELAY(0xFFFF);
        LCD_DELAY(0xFFFF);
        setCursorLCD(1, 0);
        putsLCD("                ", LCD_NOWRAP);
    }
     */
}

/*
        switch(dispMode) {
            case STATION_INFO:
                // print Band Freq: ID - SONG, ARTIST
                //snprintf(lcdStr, 32, "%s %d: %s - %s, %s", "FM", currentFreq, rdsId, rdsSong, rdsArtist);
                break;
            case FREQ_AND_TITLE:
                //snprintf(lcdStr, 32, "%s %d: %s", "FM", currentFreq, rdsSong);
                break;
            case FREQ_AND_ARTIST:
                //snprintf(lcdStr, 32, "%s %d: %s", "FM", currentFreq, rdsArtist);
                break;
            case FREQ_AND_ID:
                //snprintf(lcdStr, 32, "%s %d: %s", "FM", currentFreq, rdsId);
                break;
            case PRESETS_FM:
                //snprintf(lcdStr, 32, "%s %d: %s", "FM", currentFreq, rdsId);
                break;
            case PRESETS_AM:
                //snprintf(lcdStr, 32, "%s %d: %s", "FM", currentFreq, rdsId);
                break;
        }
 */


/*

 */