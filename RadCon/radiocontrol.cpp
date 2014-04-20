#include "radiocontrol.h"
#include "ui_radiocontrol.h"
#include "globals.h"
#include "connectiondialog.h"
#include <iostream>

RadioControl::RadioControl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RadioControl)
{
    ui->setupUi(this);
    Globals *g = Globals::instance();
    ui->fmRadioButton->setChecked(true);
    ui->amRadioButton->setChecked(false);
    ui->freqSpinner->setSingleStep(0.1);
    ui->freqSpinner->setRange(FM_MIN_FREQ, FM_MAX_FREQ);
    ui->freqSpinner->setValue(g->getFreq());

    ui->freqSlider->setSingleStep(1);
    ui->freqSlider->setRange(static_cast<int>(FM_MIN_FREQ*10),
                             static_cast<int>(FM_MAX_FREQ*10));
    ui->freqSlider->setValue(static_cast<int>(g->getFreq())*10);

    // no connection
}

RadioControl::~RadioControl()
{
    delete ui;
}

void updateTuning(void)
{
    Globals *g = Globals::instance();
    if(g->connection != nullptr) {
        sendCommand(TUNE);
    }
    else {
        std::cerr << "Please configure the connection!\n";
    }
    return;
}

void RadioControl::on_freqSpinner_valueChanged(double freq)
{
    // update it in the slider
    int asInt;
    Globals *g = Globals::instance();
    if(g->band == FM) {
        asInt = static_cast<int>(10*freq);
    }
    else {
        asInt = static_cast<int>(freq);
    }
    ui->freqSlider->setSliderPosition(asInt);

    g->setFreq(freq);
    updateTuning();
}

void RadioControl::on_freqSlider_valueChanged(int value)
{
    double asDouble;
    Globals *g = Globals::instance();
    if(g->band == FM) {
        asDouble = static_cast<double>(value)/10.;
    }
    else {
        asDouble = static_cast<double>(value);
    }
    ui->freqSpinner->setValue(asDouble);
    g->setFreq(asDouble);
}

void RadioControl::on_preset1_clicked()
{
    Globals *g = Globals::instance();
    g->preset = 1;
    sendCommand(TUNE_PRESET);
}

void RadioControl::on_preset2_clicked()
{
    Globals *g = Globals::instance();
    g->preset = 2;
    sendCommand(TUNE_PRESET);
}

void RadioControl::on_preset3_clicked()
{
    Globals *g = Globals::instance();
    g->preset = 3;
    sendCommand(TUNE_PRESET);
}

void RadioControl::on_preset4_clicked()
{
    Globals *g = Globals::instance();
    g->preset = 4;
    sendCommand(TUNE_PRESET);
}

void RadioControl::on_preset5_clicked()
{
    Globals *g = Globals::instance();
    g->preset = 5;
    sendCommand(TUNE_PRESET);
}

void RadioControl::on_preset6_clicked()
{
    Globals *g = Globals::instance();
    g->preset = 6;
    sendCommand(TUNE_PRESET);
}

void RadioControl::on_fmRadioButton_clicked()
{
    ui->amRadioButton->setChecked(false);
    // adjust the tuning range
    ui->freqSlider->setSingleStep(1);
    ui->freqSlider->setRange(static_cast<int>(FM_MIN_FREQ)*10,
                             static_cast<int>(FM_MAX_FREQ)*10);

    ui->freqSpinner->setSingleStep(0.1);
    ui->freqSpinner->setRange(FM_MIN_FREQ,
                              FM_MAX_FREQ);
    Globals *g = Globals::instance();
    g->band = FM;
    updateTuning();
}

void RadioControl::on_amRadioButton_clicked()
{
    ui->fmRadioButton->setChecked(false);
    // adjust the tuning range
    ui->freqSlider->setSingleStep(1);
    ui->freqSlider->setRange(static_cast<int>(AM_MIN_FREQ),
                             static_cast<int>(AM_MAX_FREQ));

    ui->freqSpinner->setSingleStep(1.0);
    ui->freqSpinner->setRange(AM_MIN_FREQ,
                              AM_MAX_FREQ);

    Globals *g = Globals::instance();
    g->band = AM;
    updateTuning();
}

void RadioControl::on_actionBluetooth_Port_triggered()
{
    ConnectionDialog cd;
    Utils::SerialPeer *sp = cd.start();
    Globals *g = Globals::instance();
    if(sp != nullptr) {
        if(g->connection != nullptr) {
            delete g->connection;
        }
        g->connection = sp;
    }
}

void RadioControl::on_freqSlider_sliderReleased()
{
    updateTuning();
}
