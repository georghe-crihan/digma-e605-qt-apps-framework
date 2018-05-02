#ifndef KEYPRESS_H
#define KEYPRESS_H
 
#include <QtGui/QWidget>
#include <QtGui/QtGui>
 
class KeyPress : public QWidget
{
    Q_OBJECT
public:
    KeyPress(QWidget *parent = 0);
 
protected:
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
 
private:
    QLabel *myLabel;
    QVBoxLayout *mainLayout;
};
 
#endif // KEYPRESS_H
