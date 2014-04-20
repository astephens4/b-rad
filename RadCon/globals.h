#ifndef GLOBALS_H
#define GLOBALS_H 1

#define FM_MIN_FREQ 88.0
#define FM_MAX_FREQ 107.9

#define AM_MIN_FREQ 540
#define AM_MAX_FREQ 1600

#include "Utils/SerialPeer.hpp"

enum RadioCommand {
    TUNE = 0,
    TUNE_PRESET,
    VOLUME,
    BAND,
    SET_PRESET,
    NEXT,
    DISPLAY
};

enum AdvanceMode {
    PRESET = 0,
    FREQUENCY,
    SCAN
};


enum FrequencyBand {
    FM = 0,
    AM
};

enum Direction {
    DOWN = 0,
    UP
};

enum DisplayMode {
    SCROLL_ALL = 0,
    FREQ_SONG,
    FREQ_ARTIST,
    FREQ_ID,
    SCROLL_PRESETS_FM,
    SCROLL_PRESETS_AM
};

bool sendCommand(RadioCommand cmd);

class Globals {
public:
    static Globals* instance();


   void setFreq(double freq);
   double getFreq(void);

    FrequencyBand band;
    unsigned char fm_freq;
    unsigned char am_freq;
    unsigned char preset;
    unsigned char volume;
    bool seekActive;
    AdvanceMode mode;
    Direction dir;
    DisplayMode dispMode;
    Utils::SerialPeer *connection;
private:
    Globals();

    static Globals *onlyInstance;
};

#endif // GLOBALS_H
