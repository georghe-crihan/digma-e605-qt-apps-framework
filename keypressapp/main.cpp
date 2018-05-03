#include <QtGui/QtGui>
#include "keypress.h"
 
int main(int argc, char *argv[])
{
    QApplication a(argc, argv, 0x40704); // Some wierd magic value from Boeye...
 
    KeyPress *keyPress = new KeyPress();
    keyPress->show();
 
    return a.exec();
}
