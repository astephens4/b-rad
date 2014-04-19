#include <QtGui/QApplication>
#include "radiocontrol.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RadioControl w;
    w.show();
    
    return a.exec();
}
