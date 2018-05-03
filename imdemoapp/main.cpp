#include <QApplication>
#include <QtGui/QtGui>
#include <QWSServer>
#include <Imcontext.hpp>

class MainWindow : public QMainWindow {
public:
  MainWindow() {
    setGeometry(0, 0, 599, 799);
    QWidget widget(this);
    QFont font;
    font.setWeight(75);
    font.setPixelSize(21);
    setFont(font);
    QLineEdit lineEdit(&widget);
    setCentralWidget(&widget);
    QPushButton button(&widget);
    lineEdit.setGeometry(0, 100, 499, 129);
    QLineEdit lineEdit2(&widget);
    lineEdit2.setGeometry(0, 200, 499, 229);
    button.setGeometry(0, 100, 299, 119);
    QTextEdit textEdit(&widget);
    textEdit.setGeometry(0, 300, 299, 319);
  }
};

int main(int argc, char **argv)
{
  QApplication app(argc, argv, 0x40704);
  app.setCursorFlashTime(3600000);
  MainWindow mainWindow;
  sys::Imcontext::installInputMethod();
  mainWindow.show();
  return app.exec();
}
