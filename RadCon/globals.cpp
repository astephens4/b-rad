#include "globals.h"

#include <cstdlib>
#include <vector>

Globals *Globals::onlyInstance = nullptr;

Globals* Globals::instance()
{
    if(onlyInstance == nullptr) {
        onlyInstance = new Globals();
    }

    return onlyInstance;
}

void Globals::setFreq(double freq)
{
    if(this->band == FM) {
        this->fm_freq = static_cast<unsigned int>(freq * 10) - 880;
    }
    else {
        this->am_freq = static_cast<unsigned int>(freq / 10) - 54;
    }
}

double Globals::getFreq(void)
{
    if(this->band == FM) {
        return static_cast<double>(this->fm_freq + 880) / 10.;
    }
    else {
        return static_cast<double>(this->am_freq + 54) * 10.;
    }
}

bool sendCommand(RadioCommand command)
{
    Globals *vars = Globals::instance();
    // pack the command as necessary
    std::vector<unsigned char> bytes;
    switch(command) {
    case TUNE:
        bytes.resize(2);
        bytes[0] |= (static_cast<unsigned char>(vars->band&0x01)<<4);
        if(vars->band == FM)
            bytes[1] = static_cast<unsigned char>(vars->fm_freq);
        else
            bytes[1] = static_cast<unsigned char>(vars->am_freq);
        break;
    case TUNE_PRESET:
        bytes.resize(1);
        bytes[0] = 2;
        bytes[0] |= (static_cast<unsigned char>(vars->band&0x01)<<4);
        bytes[0] |= (vars->preset&0x0F);
        break;
    case VOLUME:
        bytes.resize(1);
        bytes[0] = 10;
        bytes[0] |= static_cast<unsigned char>(vars->dir&0x01)<<4;
        break;
    case BAND:
        bytes.resize(1);
        bytes[0] = 6;
        bytes[0] |= static_cast<int>(vars->band&0x01);
        break;
    case SET_PRESET:
        bytes.resize(2);
        bytes[0] = 8;
        bytes[0] |= static_cast<unsigned char>(vars->band)<<4;
        bytes[0] |= static_cast<unsigned char>(vars->preset&0x0F);
        if(vars->band == FM)
            bytes[1] = vars->fm_freq;
        else
            bytes[1] = vars->am_freq;
        break;
    case NEXT:
        bytes.resize(1);
        bytes[0] = 10;
        bytes[0] |= static_cast<unsigned char>(vars->dir&0x01)<<4;
        bytes[0] |= static_cast<unsigned char>(vars->mode&0x03)<<2;
        break;
    case DISPLAY:
        bytes.resize(1);
        bytes[0] = 15;
        bytes[0] |= static_cast<unsigned char>(vars->dispMode&0x07)<<2;
        break;
    }

    return vars->connection->send(bytes);
}

Globals::Globals() :
    band(FM),
    fm_freq(88),
    am_freq(54),
    preset(1),
    volume(50),
    seekActive(false),
    mode(FREQUENCY),
    dir(UP),
    dispMode(SCROLL_ALL),
    connection(NULL)
{

}
