#include "Si4735.h"
#include <i2c.h>
#include "SEE.h"

#include <string.h>
#include <stdio.h>

#define I2CBRG_400KHZ 157

#define NRESET_TRIS TRISBbits.TRISB4
#define NRESET_PIN LATBbits.LATB4

#define VIOPOWER_PIN LATEbits.LATE8
#define VIOPOWER_TRIS TRISEbits.TRISE8

#define RADIO_ADDR 0x22
#define WRITE 0
#define READ 1

// Some Modes for the device
#define TUNE_FM 0
#define TUNE_AM 1
#define ANALOG_OUT 0x5

// Some offsets for data stored in NVRAM
#define SETTINGS_VALID 0x0
#define LAST_BAND 0x2
#define LAST_FREQ 0x4
#define LAST_VOLUME 0x6
#define FM_PRESET_BEGIN 0x10
#define AM_PRESET_BEGIN 0x20

// Memory related constants
#define MEMORY_CONTENTS_VALID_MAGIC 0xBEEF
#define NUM_PRESETS 6

enum Band currentBand;
int currentVolume;
int currentPreset;
int currentFreq;

char rdsSong[34];
char rdsArtist[34];
char rdsId[10];


#define MAX_CMD_ARGS 7
#define MAX_CMD_RESP 16
char cmdArgs[MAX_CMD_ARGS+1]; // +1 to align with word size
char cmdResp[MAX_CMD_RESP];

struct RSQ g_tuneStatus;

union Status lastReceived;

#define BeginTX() \
StartI2C1(); \
IdleI2C1(); \
MasterWriteI2C1(RADIO_ADDR | WRITE); \
IdleI2C1()

#define EndTX() StopI2C1(); IdleI2C1()

#define BeginRX()\
StartI2C1(); \
IdleI2C1(); \
MasterWriteI2C1(RADIO_ADDR | READ); \
IdleI2C1()

#define EndRX() StopI2C1(); IdleI2C1()

#define SendByte(byte) MasterWriteI2C1(byte); IdleI2C1()

#define GetByte() MasterReadI2C1()

#define KeepReading() AckI2C1(); IdleI2C1()

#define DoneReading() NotAckI2C1(); IdleI2C1()

/**
 * 
 */
void waitForCTS(void)
{
    while(1) {
        BeginRX();

        lastReceived.i = GetByte();
        DoneReading();

        EndRX();

        if(lastReceived.radio.CTS == 1) {
            return;
        }
    }
}

/**
 * 
 */
void waitForSTC(void)
{
    while(1) {
        BeginRX();

        lastReceived.i = GetByte();
        DoneReading();

        EndRX();

        if(lastReceived.radio.STCINT == 1) {
            return;
        }
    }
}

void sendCommand(enum RadioCommand cmd,
                 char *args, int nargs,
                 char *resp, int nresp)
{
    if(nargs > MAX_CMD_ARGS)
        nargs = MAX_CMD_ARGS;
    if(nresp > MAX_CMD_RESP)
        nresp = MAX_CMD_RESP;

    waitForCTS();
    // send command
    BeginTX();    
    SendByte(cmd);
    int i = 0;
    for(i = 0; i < nargs; ++i) {
        SendByte(args[i]);
    }
    EndTX();

    waitForCTS();
    // get responses
    BeginRX();
    lastReceived.i = GetByte();
    KeepReading();

    for(i = 0; i < nresp; ++i) {
        resp[i] = GetByte();
        if(i < nresp-1) {
            KeepReading();
        }
    }
    DoneReading();
    EndRX();

    return;
}

int getProperty(enum RadioProperty property)
{
    cmdArgs[0] = 0;
    cmdArgs[1] = (property&0xFF00) >> 8;
    cmdArgs[2] = property&0xFF;
    sendCommand(GET_PROPERTY, cmdArgs, 3, cmdResp, 3);
    return (int)cmdResp[1] << 8 | (int)cmdResp[2];
}

void setProperty(enum RadioProperty property, int value)
{
    cmdArgs[0] = 0;
    cmdArgs[1] = property&0xFF00 >> 8;
    cmdArgs[2] = property&0xFF;
    cmdArgs[3] = value&0xFF00 >> 8;
    cmdArgs[4] = value&0xFF;
    sendCommand(SET_PROPERTY, cmdArgs, 5, cmdResp, 0);
}

void configure(void)
{
    // enable RDS and STC interrupts
    setProperty(GPO_IEN, 0x5);

    // set some seek properties
    setProperty(FM_SEEK_TUNE_SNR_THRESHOLD, 3);
    setProperty(FM_SEEK_TUNE_RSSI_THRESHOLD, 20);

    // set some US specific properties
    setProperty(FM_RDS_INT_SOURCE, 0x4); // RDS Interrupt

    // Enable the RDS and allow all blocks so we can compute the error
    // rate later.
    setProperty(FM_RDS_CONFIG, 0x1 |
                    (3 << 14) |
                    (3 << 12) |
                    (3 << 10) |
                    (3 << 8));

    // 75 us for USA
    setProperty(FM_DEEMPHASIS, 0x2);
}

/**
 * Initialize the first SI4735 module
 */
void initRadio(void)
{
    OpenI2C1(I2C_ON &
             I2C_IDLE_CON &
             I2C_IPMI_DIS &
             I2C_7BIT_ADD &
             I2C_SLW_EN &
             I2C_GCALL_DIS &
             I2C_STR_DIS &
             I2C_ACK,
             I2CBRG_400KHZ);

    // initialize power and reset
    NRESET_TRIS = 0;
    NRESET_PIN = 0;

    VIOPOWER_TRIS = 0;
    VIOPOWER_PIN = 0;

    // Let's make timer2 our timer
    T2CON = 0x8030;
    TMR2 = 0;

    strncpy(rdsSong, "Song", 33);
    strncpy(rdsArtist, "Artist", 33);
    strncpy(rdsId, "WXIX", 9);

    // while waiting at least 250us, read data in from NVRAM
    InitSEE();

    int settingsValid = iReadSEE(SETTINGS_VALID);
    if(settingsValid == MEMORY_CONTENTS_VALID_MAGIC) {
        currentBand = iReadSEE(LAST_BAND);
        currentFreq = iReadSEE(LAST_FREQ);
        currentVolume = iReadSEE(LAST_VOLUME);
    }
    else {
        currentBand = FM;
        currentFreq = 9650;
        currentVolume = 50;
        // write default preset values and last used into NVRAM
        iWriteSEE(LAST_BAND, currentBand);
        iWriteSEE(LAST_FREQ, currentFreq);
        iWriteSEE(LAST_VOLUME, currentVolume);

        // Write the min freqs into the presets
        int i = 0;
        for(i = 0; i < NUM_PRESETS; ++i) {
            iWriteSEE(FM_PRESET_BEGIN+(2*i), FM_MIN_FREQ);
        }

        for(i = 0; i < NUM_PRESETS; ++i) {
            iWriteSEE(AM_PRESET_BEGIN+(2*i), AM_MIN_FREQ);
        }

        iWriteSEE(SETTINGS_VALID, MEMORY_CONTENTS_VALID_MAGIC);
    }

    removePower();
    powerOn(currentBand);
    configure();
}

/**
 * Remove power from the device and hold in reset
 */
void removePower(void)
{
    // since we will be putting it in reset, sending the shutdown command
    // won't pose issues
    shutDown();

    // place in reset
    NRESET_PIN = 0;

    // wait ~150 us
    TMR2 = 0;
    while(TMR2 < 10);

    // remove power
    VIOPOWER_PIN = 0;
}

/**
 * Soft shutdown of device
 */
void shutDown(void)
{
    sendCommand(POWER_DOWN, NULL, 0, NULL, 0);
}

/**
 * Soft power up of radio
 * @param band Turn it on in FM or AM mode
 */
void powerOn(enum Band band)
{
    // ensure that there is at least 100 us beore enabling the power
    TMR2 = 0;
    while(TMR2 < 10);
    VIOPOWER_PIN = 1;

    // and then at least 250 us before taking out of reset
    TMR2 = 0;
    while(TMR2 < 16);
    NRESET_PIN = 1;

    TMR2 = 0;
    while(TMR2 < 1);

    cmdArgs[0] = band;
    cmdArgs[1] = ANALOG_OUT;
    sendCommand(POWER_UP, cmdArgs, 2, NULL, 0);

}

/**
 * Tune into the station on the specified band
 * @param band AM or FM. Changing the band requires a soft reset of the device
 * @param freq Frequency to tune to
 * @return True if tuned, false otherwise. Should only be false if the provided
 *         frequency is invalid
 */
int tuneStation(enum Band band, int freq)
{
    if(band != currentBand) {
        shutDown();
        powerOn(band);
        currentBand = band;
        iWriteSEE(LAST_BAND, band);
    }
    if((band == FM) && (freq < FM_MIN_FREQ || freq > FM_MAX_FREQ)) {
        return 0;
    }
    else if((band == AM) && (freq < AM_MIN_FREQ || freq > AM_MAX_FREQ)) {
        return 0;
    }
    currentFreq = freq;
    
    // Poll status until CTS is high
    waitForCTS();

    cmdArgs[0] = 0;
    cmdArgs[1] = freq&0xFF00 >> 8;
    cmdArgs[2] = freq&0xFF;
    cmdArgs[3] = 0;
    sendCommand(FM_TUNE_FREQ, cmdArgs, 4, NULL, 0);

    // Wait for CTS so we can check STC
    waitForCTS();
    waitForSTC();

    // get the tune status
    g_tuneStatus = tuneStatus();

    iWriteSEE(LAST_FREQ, freq);
    return 1;
}

/**
 * Tune into the given preset on the specified band
 * @param band AM or FM. Changing the band requires a soft reset of the device
 * @param preset Value 0-5, from memory
 */
void tunePreset(enum Band band, int preset)
{
    int freq;
    if(band == FM) {
        freq = iReadSEE(FM_PRESET_BEGIN+(preset*2));
    }
    else {
        freq = iReadSEE(AM_PRESET_BEGIN+(preset*2));
    }
    tuneStation(band, freq);
}

/**
 * Store the specified preset in memory
 * @param band
 * @param preset
 * @param freq
 */
void setPreset(enum Band band, int preset, int freq)
{
    if(band == FM) {
        iWriteSEE(FM_PRESET_BEGIN+(preset*2), freq);
    }
    else {
        iWriteSEE(AM_PRESET_BEGIN+(preset*2), freq);
    }
}

/**
 * Find the next station with good enough signal quality on the band.
 * @param band
 * @param dir
 * @param wrap If the edge of the range is hit, go to the other end
 */
void seek(enum Band band, enum Direction dir, int wrap)
{
    if(band != currentBand) {
        shutDown();
        powerOn(band);
        currentBand = band;
        iWriteSEE(LAST_BAND, band);
    }

    cmdArgs[0] = dir << 4 | wrap << 3;
    sendCommand(FM_SEEK_START, cmdArgs, 1, NULL, 0);

    tuneStatus();
    iWriteSEE(LAST_FREQ, g_tuneStatus.freq);
}

/**
 * Check the status of the currently tuned station
 * @return The signal quality and freqency
 */
struct RSQ tuneStatus(void)
{
    cmdArgs[0] = 1;
    sendCommand(FM_TUNE_STATUS, cmdArgs, 1, cmdResp, 7);

    // byte 0 is a bunch of flags

    // byte 1 is the high byte of the tuned frequency
    g_tuneStatus.freq = (int)cmdResp[1] << 8;

    // byte 2 is the low byte of the tuned frequency
    g_tuneStatus.freq |= (int)cmdResp[2];

    // byte 3 is the Received Signal Strength Indicator
    g_tuneStatus.rssi = cmdResp[3];

    // byte 4 is the Signal to Noise Ratio
    g_tuneStatus.snr = cmdResp[4];

    // byte 5 is the multipath interference measurement
    g_tuneStatus.multi = cmdResp[5];

    // byte 6 is the antenna cap adjusted value

    return g_tuneStatus;
}

/**
 * Get the chip's info for debugging purposes
 * @return Chip info
 */
struct RevInfo getRevisionInfo(void)
{
    sendCommand(GET_REV, NULL, 0, cmdResp, 8);

    struct RevInfo ret;
    ret.ptNum = cmdResp[0];
    ret.fwmajor = cmdResp[1];
    ret.fwminor = cmdResp[2];
    ret.patch = (cmdResp[3]) << 8;
    ret.patch |= cmdResp[4];
    ret.cmpmajor = cmdResp[5];
    ret.cmpminor = cmdResp[6];
    ret.chiprev = cmdResp[7];
    return ret;
}

/**
 * Set the volume to the specified value
 * @param volume 0-63
 */
void setVolume(unsigned char volume)
{
    // ensure the mute is off
    mute(0);
    if(volume > 63)
        volume = 63;
    setProperty(RX_VOLUME, volume);
    iWriteSEE(LAST_VOLUME, volume);
    return;
}

/**
 *
 * @return current volume level
 */
unsigned char getVolume(void)
{
    return (unsigned char)(getProperty(RX_VOLUME)&0xFF);
}

/**
 * Turn sound on or off
 * @param quiet When set, turn mute on
 */
void mute(int quiet)
{
    if(quiet) {
        setProperty(RX_HARD_MUTE, 3);
    }
    else {
        setProperty(RX_HARD_MUTE, 0);
    }
}

/**
 *
 * @return The value of the current status register
 */
struct RadioStatus getStatus(void)
{
    BeginRX();
    lastReceived.i = GetByte();
    DoneReading();
    EndRX();
    return lastReceived.radio;
}