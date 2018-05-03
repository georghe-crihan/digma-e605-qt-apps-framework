#include <stdio.h> // fprintf()
#include <QApplication>
#include <QtGui/QtGui>
#include <QWSServer>
#include <Imcontext.hpp>

class MainWindow : public QMainWindow {
public:
  void simple() {
    setGeometry(0, 0, 599, 799);
    QWidget widget(this);
    setCentralWidget(&widget);
    ui::LineEdit lineEdit(&widget);
    lineEdit.setGeometry(0, 100, 499, 129);
  };

  void hairy() {
    setGeometry(0, 0, 599, 799);
    ui::BoeyeLine line(3, ui::BoeyeLine::Vertical, this);
    QWidget widget(this);
    QFont font;
    font.setWeight(75);
    font.setPixelSize(21);
    setFont(font);
    ui::LineEdit lineEdit(&widget);
    setCentralWidget(&widget);
    QPushButton button(&widget);
    lineEdit.setGeometry(0, 100, 499, 129);
    ui::LineEdit lineEdit2(&widget);
    lineEdit2.setGeometry(0, 200, 499, 229);
    button.setGeometry(0, 100, 299, 119);
    QTextEdit textEdit(&widget);
    textEdit.setGeometry(0, 300, 299, 319);
  };
};

int main(int argc, char **argv)
{
  QApplication app(argc, argv, 0x40704);
  MainWindow mainWindow;
  if (argc >= 2) {
    app.setCursorFlashTime(3600000);
//    sys::Imcontext::installInputMethod();
//    ui::InputDialog dialog;
//    dialog.inputtext();
    mainWindow.hairy();
    sys::ProxyScreen screen;
    screen.flush(sys::ProxyScreen::FULL);
  } else {
    mainWindow.simple();
  }
  sys::SysStatus status;
  fprintf(stderr, "Voltage=%d\n", status.batteryVoltage());
  mainWindow.show();
  return app.exec();
}
