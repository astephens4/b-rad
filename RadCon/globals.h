#ifndef GLOBALS_H
#define GLOBALS_H 1

#include <Utils/SerialPeer.hpp>

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
    Globals* instance(const std::string& uartCon);


   void setFreq(double freq);

    FrequencyBand band;
    unsigned char freq;
    unsigned char preset;
    unsigned char volume;
    bool seekActive;
private:
    Globals(const std::string& uartCon);
    Utils::SerialPeer connection;

    Globals *onlyInstance;
};

#endif // GLOBALS_H
