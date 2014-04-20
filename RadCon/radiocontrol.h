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

    void on_freqSlider_valueChanged(int value);

    void on_preset1_clicked();
    void on_preset2_clicked();
    void on_preset3_clicked();
    void on_preset4_clicked();
    void on_preset5_clicked();
    void on_preset6_clicked();

    void on_fmRadioButton_clicked();
    void on_amRadioButton_clicked();

    void on_actionBluetooth_Port_triggered();

    void on_freqSlider_sliderReleased();

private:
    Ui::RadioControl *ui;
};

#endif // RADIOCONTROL_H
