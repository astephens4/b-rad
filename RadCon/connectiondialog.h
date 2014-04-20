#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>

#include "Utils/SerialPeer.hpp"

namespace Ui {
class ConnectionDialog;
}

class ConnectionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConnectionDialog(QWidget *parent = 0);
    ~ConnectionDialog();
    
    Utils::SerialPeer* start(void);
private:
    Ui::ConnectionDialog *ui;
};

#endif // CONNECTIONDIALOG_H
