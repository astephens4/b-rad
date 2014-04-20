#include "connectiondialog.h"
#include "ui_connectiondialog.h"

#include <QStringList>
#include <iostream>

ConnectionDialog::ConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionDialog)
{
    ui->setupUi(this);
    QStringList baudrates;
    baudrates.push_back(QString("9600"));
    baudrates.push_back(QString("19200"));
    baudrates.push_back(QString("38400"));
    baudrates.push_back(QString("57600"));
    baudrates.push_back(QString("115200"));
    baudrates.push_back(QString("230400"));
    ui->baudCombo->addItems(baudrates);

    QStringList parityItems;
    parityItems.push_back(QString("None"));
    parityItems.push_back(QString("Odd"));
    parityItems.push_back(QString("Even"));
    ui->parityCombo->addItems(parityItems);

    QStringList flowControl;
    flowControl.push_back(QString("None"));
    flowControl.push_back(QString("CTS/RTS"));
    ui->flowControlCombo->addItems(flowControl);
}

ConnectionDialog::~ConnectionDialog()
{
    delete ui;
}

Utils::SerialPeer* ConnectionDialog::start(void)
{
    int result = this->exec();
    if(result == 0) {
        return nullptr;
    }
    Utils::SerialPeer::Settings settings;
    settings.speed = Utils::SerialPeer::BAUD_115200;
    settings.mode = Utils::SerialPeer::UART_8N1;
    settings.flow = Utils::SerialPeer::UART_NO_FLOW_CONTROL;
    settings.blocking = Utils::SerialPeer::UART_FULL_BLOCK;
    return new Utils::SerialPeer(ui->uartName->text().toUtf8().constData(), settings);
}
