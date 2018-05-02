#include "keypress.h"
#include <QtGui/QApplication>
#include <QtGui/QKeyEvent>
 
KeyPress::KeyPress(QWidget *parent) :
    QWidget(parent)
{
    myLabel = new QLabel("LABEL");
    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(myLabel);
    setLayout(mainLayout);
 
}
 
void KeyPress::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        myLabel->setText("You pressed ESC");
        break;
    case Qt::Key_Return:
        myLabel->setText("You pressed RETURN");
        break;
    case Qt::Key_Up:
        myLabel->setText("You pressed UP ARROW");
        break;
    default:
        myLabel->setText("You pressed " + QKeySequence(event->key()).toString());
    }
}
 
void KeyPress::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Escape:
        myLabel->setText("You released ESC");
        break;
    case Qt::Key_Return:
        myLabel->setText("You released RETURN");
        break;
    case Qt::Key_Up:
        myLabel->setText("You released UP ARROW");
        break;
    default:
        myLabel->setText("You released " + QKeySequence(event->key()).toString());
    }
}
