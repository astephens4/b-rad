/* 
 * File:   Si4735.h
 * Author: alex
 *
 * Created on April 11, 2014, 9:24 PM
 */

#ifndef SI4735_H
#define	SI4735_H

#ifdef	__cplusplus
extern "C" {
#endif

#define FM_MIN_FREQ 8750
#define FM_MAX_FREQ 10790

#define AM_MIN_FREQ 540
#define AM_MAX_FREQ 1600

enum Band {
    FM = 0,
    AM
};

enum Direction {
    DOWN = 0,
    UP
};

enum Mode {
    PRESET = 0,
    FREQUENCY,
    SCAN
};

struct RadioStatus {
    unsigned STCINT : 1;
    unsigned : 1;
    unsigned RDSINT : 1;
    unsigned RSQINT : 1;
    unsigned : 2;
    unsigned ERR : 1;
    unsigned CTS : 1;
};

struct RevInfo {
    unsigned char ptNum;
    unsigned char fwmajor;
    unsigned char fwminor;
    int patch;
    unsigned char cmpmajor;
    unsigned char cmpminor;
    unsigned char chiprev;
};

union Status {
    struct RadioStatus radio;
    unsigned int i;
};

struct RSQ {
    unsigned int freq;
    unsigned char snr;
    unsigned char rssi;
    unsigned char multi;
};

/**
 * Initialize the first SI4735 module
 */
void initRadio(void);

/**
 * Remove power from the device and hold in reset
 * for a little bit before powering back up
 */
void removePower(void);

/**
 * Soft shutdown of device
 */
void shutDown(void);

/**
 * Soft power up of radio
 * @param band Turn it on in FM or AM mode
 */
void powerOn(enum Band band);

/**
 * Tune into the station on the specified band
 * @param band AM or FM. Changing the band requires a soft reset of the device
 * @param freq Frequency to tune to
 * @return True if tuned, false otherwise. Should only be false if the provided
 *         frequency is invalid
 */
int tuneStation(enum Band band, int freq);

/**
 * Tune into the given preset on the specified band
 * @param band AM or FM. Changing the band requires a soft reset of the device
 * @param preset Value 0-5, from memory
 */
void tunePreset(enum Band band, int preset);

/**
 * Store the specified preset in memory
 * @param band
 * @param preset
 * @param freq
 */
void setPreset(enum Band band, int preset, int freq);

/**
 * Find the next station with good enough signal quality on the band.
 * @param band
 * @param dir
 * @param wrap If the edge of the range is hit, go to the other end
 */
void seek(enum Band band, enum Direction dir, int wrap);

/**
 * Check the status of the currently tuned station
 * @return The signal quality and freqency
 */
struct RSQ tuneStatus(void);

/**
 * Get the chip's info for debugging purposes
 * @return Chip info
 */
struct RevInfo getRevisionInfo(void);

/**
 * Set the volume to the specified value
 * @param volume 0-63
 */
void setVolume(unsigned char volume);

/**
 *
 * @return current volume level
 */
unsigned char getVolume(void);

/**
 * Turn sound on or off
 * @param quiet When set, turn mute on
 */
void mute(int quiet);

/**
 *
 * @return The value of the current status register
 */
struct RadioStatus getStatus(void);

enum RadioCommand {
    POWER_UP = 0x01,
    GET_REV = 0x10,
    POWER_DOWN = 0x11,
    SET_PROPERTY = 0x12,
    GET_PROPERTY = 0x13,
    GET_INT_STATUS = 0x14,
    // PURPOSELY LEFT OUT PATCH COMMANDS
    FM_TUNE_FREQ = 0x20,
    FM_SEEK_START = 0x21,
    FM_TUNE_STATUS = 0x22,
    FM_RSQ_STATUS = 0x23,
    FM_RDS_STATUS = 0x24,
    FM_AGC_STATUS = 0x27,
    FM_AGC_OVERRIDE = 0x28,
    GPIO_CTL = 0x80,
    GPIO_SET = 0x81,
};

enum RadioProperty {
    GPO_IEN = 0x0001,
    DIGITAL_OUTPUT_FORMAT = 0x0102,
    DIGITAL_OUTPUT_SAMPLE_RATE = 0x0104,
    REFCLK_FREQ = 0x0201,
    REFCLK_PRESCALE = 0x0202,
    FM_DEEMPHASIS = 0x1100,
    FM_CHANNEL_FILTER = 0x1102,
    FM_BLEND_STEREO_THRESHOLD = 0x1105,
    FM_BLEND_MONO_THRESHOLD = 0x1106,
    FM_ANTENNA_INPUT = 0x1107,
    FM_MAX_TUNE_ERROR = 0x1108,
    FM_RSQ_INT_SOURCE = 0x1200,
    FM_RSQ_SNR_HI_THRESHOLD = 0x1201,
    FM_RSQ_SNR_LO_THRESHOLD = 0x1202,
    FM_RSQ_RSSI_HI_THRESHOLD = 0x1203,
    FM_RSQ_RSSI_LO_THRESHOLD = 0x1204,
    FM_RSQ_MULTIPATH_HI_THRESHOLD = 0x1205,
    FM_RSQ_MULTIPATH_LO_THRESHOLD = 0x1206,
    FM_RSQ_BLEND_THRESHOLD = 0x1207,
    FM_SOFT_MUTE_RATE = 0x1300,
    FM_SOFT_MUTE_SLOPE = 0x1301,
    FM_SOFT_MUTE_MAX_ATTENUATION = 0x1302,
    FM_SOFT_MUTE_SNR_THRESHOLD = 0x1303,
    FM_SOFT_MUTE_RELEASE_RATE = 0x1304,
    FM_SOFT_MUTE_ATTACK_RATE = 0x1305,
    FM_SEEK_BAND_BOTTOM = 0x1400,
    FM_SEEK_BAND_TOP = 0x1401,
    FM_SEEK_FREQ_SPACING = 0x1402,
    FM_SEEK_TUNE_SNR_THRESHOLD = 0x1403,
    FM_SEEK_TUNE_RSSI_THRESHOLD = 0x1404,
    FM_RDS_INT_SOURCE = 0x1500,
    FM_RDS_INT_FIFO_COUNT = 0x1501,
    FM_RDS_CONFIG = 0x1502,
    FM_RDS_CONFIDENCE = 0x1503,
    FM_AGC_ATTACK_RATE = 0x1700,
    FM_AGC_RELEASE_RATE = 0x1701,
    FM_BLEND_RSSI_STEREO_THRESHOLD = 0x1800,
    FM_BLEND_RSSI_MONO_THRESHOLD = 0x1801,
    FM_BLEND_RSSI_ATTACK_RATE = 0x1802,
    FM_BLEND_RSSI_RELEASE_RATE= 0x1803,
    FM_BLEND_SNR_STEREO_THRESHOLD = 0x1804,
    FM_BLEND_SNR_MONO_THRESHOLD = 0x1805,
    RX_VOLUME = 0x4000,
    RX_HARD_MUTE = 0x4001
};

#ifdef	__cplusplus
}
#endif

#endif	/* SI4735_H */

