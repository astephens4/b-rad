#include "globals.h"

Globals* Globals::instance(const std::string &uartCon)
{
    if(onlyInstance == nullptr) {
        onlyInstance = new Globals(uartCon);
    }

    return onlyInstance;
}

void Globals::setFreq(double freq)
{
    this->freq = static_cast<unsigned int>(freq * 10) - 880;
}

void sendCommand(RadioCommand command)
{
    Globals *vars = Globals::instance("derp");
    // pack the command as necessary
    std::vector<unsigned char> bytes;
    switch(command) {
    case TUNE:
        bytes.resize(2);
        bytes[0] |= (static_cast<unsigned char>(vars->band)<<4);
        bytes[1] = static_cast<unsigned char>(vars->freq);
        break;
    case TUNE_PRESET:
        bytes[0] = 2;
        bytes[0] |= (static_cast<unsigned char>(vars->band)<<4);
        bytes[0] |= // the preset as a 4 bit value
        break;
    case VOLUME:

        break;
    case BAND:

        break;
    case SET_PRESET:

        break;
    case NEXT:

        break;
    case DISPLAY:

        break;
    }
}

Globals::Globals(const std::string& uartCon) :
    connection(uartCon, settings)
{

}
