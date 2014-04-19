#ifndef RADIOCONTROL_H
#define RADIOCONTROL_H

#include <QMainWindow>

namespace Ui {
class RadioControl;
}

class RadioControl : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit RadioControl(QWidget *parent = 0);
    ~RadioControl();
    
private slots:
    void on_freqSpinner_valueChanged(double arg1);

private:
    Ui::RadioControl *ui;
};

#endif // RADIOCONTROL_H
