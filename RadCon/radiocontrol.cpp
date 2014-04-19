#include "radiocontrol.h"
#include "ui_radiocontrol.h"

RadioControl::RadioControl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RadioControl)
{
    ui->setupUi(this);
}

RadioControl::~RadioControl()
{
    delete ui;
}

void RadioControl::on_freqSpinner_valueChanged(double freq)
{
    Globals *g = Globals::instance();

}
