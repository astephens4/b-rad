#include "Si4735.h"
#include <i2c.h>
#include "SEE.h"

#define I2CBRG_400KHZ 37

#define NRESET_TRIS TRISBbits.TRISB4
#define NRESET_PIN PORTBbits.RB4

#define RADIO_ADDR 0xC6

// Some offsets for data stored in NVRAM
#define SETTINGS_VALID 0x0
#define LAST_BAND 0x2
#define LAST_FREQ 0x4
#define LAST_VOLUME 0x6
#define FM_PRESET_BEGIN 0x8
#define AM_PRESET_BEGIN 0x14

int settingsValid;
int currentFreq;
enum Band currentBand;
int currentVolume;
int currentPreset;

union Status {
    struct RadioStatus radio;
    unsigned int i;
} lastReceived;

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

    NRESET_TRIS = 0;
    NRESET_PIN = 0;
    // while waiting at least 250us, read data in from NVRAM
    InitSEE();
    settingsValid = iReadSEE(SETTINGS_VALID);
    if(settingsValid & 0x1) {
        currentBand = iReadSEE(LAST_BAND);
        currentFreq = iReadSEE(LAST_FREQ);
        currentVolume = iReadSEE(LAST_VOLUME);
    }
    else {
        currentBand = FM;
        currentFreq = (965-880);
        currentVolume = 50;
    }

    StartI2C1();
    IdleI2C1();

    MasterWriteI2C1(RADIO_ADDR | 1);
    IdleI2C1();

    if(I2C1STATbits.ACKSTAT != 0) {
        int oughoh = 1;
        oughoh += 3;
    }

    MasterWriteI2C1(GET_REV);
    IdleI2C1();

    if(I2C1STATbits.ACKSTAT != 0) {
        int oughoh = 1;
        oughoh += 3;
    }

    lastReceived.i = MasterReadI2C1();
    IdleI2C1();

    AckI2C1();

    int i = 0;
    for(i = 0; i < 15; ++i) {
        int val = MasterReadI2C1();
        IdleI2C1();
        AckI2C1();
    }

    if(currentBand == FM) {
        launchFM();
    }
    else {
        launchAM();
    }
}

void shutDown(void)
{
    while(1) {
        StartI2C1();
        MasterWriteI2C1(RADIO_ADDR);
        IdleI2C1();

        if(I2C1STATbits.ACKSTAT == 0) {
            break;
        }

        StopI2C1();
        IdleI2C1();
    }

    // tell it to power down
    MasterWriteI2C1(POWER_DOWN);
    IdleI2C1();

    // wait at least 10 ns before setting NRESET_PIN low
    __asm("nop");
    NRESET_PIN = 0;
}

void launchFM(void)
{
    shutDown();

    // wait ~400 us
    T1CON = 0x8030;
    TMR1 = 0;
    while(TMR1 < 400) {}

    NRESET_PIN = 1;
    while(1) {
        StartI2C1();
        MasterWriteI2C1(RADIO_ADDR);
        IdleI2C1();

        if(I2C1STATbits.ACKSTAT == 0) {
            break;
        }

        StopI2C1();
        IdleI2C1();
    }

    // tell it to power up
    MasterWriteI2C1(POWER_UP);
    IdleI2C1();

    // and receive FM
    MasterWriteI2C1(0x0);
    IdleI2C1();

    // And use analog output
    MasterWriteI2C1(0x5);
    IdleI2C1();

    // Read status response
    lastReceived.i = MasterReadI2C1();
    IdleI2C1();

    // End
    StopI2C1();
    IdleI2C1();
}

void launchAM(void)
{
    shutDown();

    // wait ~400 us
    T1CON = 0x8030;
    TMR1 = 0;
    while(TMR1 < 400) {}

    NRESET_PIN = 1;
    while(1) {
        StartI2C1();
        MasterWriteI2C1(RADIO_ADDR);
        IdleI2C1();

        if(I2C1STATbits.ACKSTAT == 0) {
            break;
        }

        StopI2C1();
        IdleI2C1();
    }

    // tell it to power up
    MasterWriteI2C1(POWER_UP);
    IdleI2C1();

    // and receive AM
    MasterWriteI2C1(0x1);
    IdleI2C1();

    // And use analog output
    MasterWriteI2C1(0x5);
    IdleI2C1();

    // Read status response
    lastReceived.i = MasterReadI2C1();
    IdleI2C1();

    // End
    StopI2C1();
    IdleI2C1();
}

void waitForCTS(void)
{
    while(1) {
        StartI2C1();
        IdleI2C1();

        MasterWriteI2C1(RADIO_ADDR | 1);
        IdleI2C1();

        MasterWriteI2C1(GET_INT_STATUS);
        IdleI2C1();

        lastReceived.i = MasterReadI2C1();
        IdleI2C1();

        AckI2C1();
        IdleI2C1();

        StopI2C1();
        IdleI2C1();
        
        if(lastReceived.radio.CTS == 1) {
            return;
        }
    }
}

void tuneStation(enum Band band, int freq)
{
    if(band != currentBand) {
        if(band == FM) {
            launchFM();
        }
        else {
            launchAM();
        }
        currentBand = band;
    }

    // Poll status until CTS is high
    waitForCTS();

    StartI2C1();
    IdleI2C1();

    MasterWriteI2C1(RADIO_ADDR);
    IdleI2C1();

    MasterWriteI2C1(FM_TUNE_FREQ);
    IdleI2C1();

    // Enable FREEZE for better sounding transitions
    MasterWriteI2C1(2);
    IdleI2C1();

    // Write the high byte of the frequency
    MasterWriteI2C1((freq&0xFF00) >> 8);
    IdleI2C1();

    // Write the low byte of the frequency
    MasterWriteI2C1(freq&0xFF);
    IdleI2C1();

    // Automatic antenna tuning cap adjustment
    MasterWriteI2C1(0);
    IdleI2C1();

    // Read the status response
    lastReceived.i = MasterReadI2C1();
    IdleI2C1();

    // Inform of receipt of status
    AckI2C1();
    IdleI2C1();

    // End transmission
    StopI2C1();
    IdleI2C1();
}

void seek(enum Band band, enum Direction dir)
{
    if(band != currentBand) {
        if(band == FM) {
            launchFM();
        }
        else {
            launchAM();
        }
        currentBand = band;
    }

    // Poll status until CTS is high
    waitForCTS();

    StartI2C1();
    IdleI2C1();

    MasterWriteI2C1(RADIO_ADDR);
    IdleI2C1();

    MasterWriteI2C1(FM_SEEK_START);
    IdleI2C1();

    // Set the direction
    MasterWriteI2C1(dir << 4);
    IdleI2C1();

    // Read the status response
    lastReceived.i = MasterReadI2C1();
    IdleI2C1();

    // Inform of receipt of status
    AckI2C1();
    IdleI2C1();

    // End transmission
    StopI2C1();
    IdleI2C1();
}

int tuneStatus(void)
{
    // Poll status until CTS is high
    waitForCTS();

    StartI2C1();
    IdleI2C1();

    MasterWriteI2C1(RADIO_ADDR);
    IdleI2C1();

    MasterWriteI2C1(FM_TUNE_STATUS);
    IdleI2C1();

    // Don't cancel or clear interrupt status
    MasterWriteI2C1(0);
    IdleI2C1();

    // Read the status response
    lastReceived.i = MasterReadI2C1();
    IdleI2C1();

    // Inform of receipt of status
    AckI2C1();
    IdleI2C1();

    // Read the first byte of the response
    int blah = MasterReadI2C1();
    IdleI2C1();

    // Inform of receipt of status
    AckI2C1();
    IdleI2C1();

    // Read the high byte of frequency
    int freq = MasterReadI2C1() << 8;
    IdleI2C1();

    // Inform of receipt of status
    AckI2C1();
    IdleI2C1();

    // Read the low byte of frequency
    freq |= MasterReadI2C1();
    IdleI2C1();

    // Inform of receipt of status
    AckI2C1();
    IdleI2C1();

    // Read the RSSI value
    int rssi = MasterReadI2C1();
    IdleI2C1();

    // Inform of receipt of status
    AckI2C1();
    IdleI2C1();

    // Read the SNR
    int snr = MasterReadI2C1();
    IdleI2C1();

    // Inform of receipt of status
    AckI2C1();
    IdleI2C1();

    // Read the Multipath thingy
    int multi = MasterReadI2C1();
    IdleI2C1();

    // Inform of receipt of status
    AckI2C1();
    IdleI2C1();

    // Read the SNR
    int something = MasterReadI2C1();
    IdleI2C1();

    // Inform of receipt of status
    AckI2C1();
    IdleI2C1();

    // End transmission
    StopI2C1();
    IdleI2C1();
}