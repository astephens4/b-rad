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

enum Band {
    FM = 0,
    AM
};

enum Direction {
    DOWN = 0,
    UP
};

/**
 * Initialize the first SI4735 module
 */
void initRadio(void);

void shutDown(void);

void launchFM(void);
void launchAM(void);

void tuneStation(enum Band band, int freq);
void seek(enum Band band, enum Direction dir);

void setVolume(int val);

enum RadioCommands {
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

enum RadioProperties {
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
    FM_SEEK_TUNE_RSSI_TRESHOLD = 0x1404,
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

struct RadioStatus {
    unsigned CTS : 1;
    unsigned ERR : 1;
    unsigned : 2;
    unsigned RSQINT : 1;
    unsigned RDSINT : 1;
    unsigned : 1;
    unsigned STCINT : 1;
};

#ifdef	__cplusplus
}
#endif

#endif	/* SI4735_H */

