#ifndef IMCONTEXT_HPP_INCLUDED
#define IMCONTEXT_HPP_INCLUDED

class QWSServer;

namespace sys { // libboeye.so

class SysStatus {
public:
  bool setSystemBusy(bool);
  int batteryVoltage();
};

class BoeyeDevice {
public:
  QString WaveForm();
};

class ProxyScreen {
public:
  enum Waveform { LOCAL = 1, PART, FULL = 3 };
  void init();
  // just an interface to the IOCTL:
  //  ioctl(framebufferdev, intarg, voidarg);
  void updateScreen(int intarg, void *voidarg);
  // does updateScreen(0x120E, voidarg); // partial
  void flush(QWidget const*, Waveform);
  void flush(QRect const&, Waveform);
  // does updateScreen(0x120D, Waveform); // full
  void flush(Waveform);
  void flushScreen(Waveform);
  void flushRegion(QRect const&, Waveform);
  void flushWidget(QWidget const*, Waveform);
};

class Imcontext : public QWSServer { // libboeyeim.so
public:
  static void installInputMethod(void);
};

}

class QLineEdit;
class QDialog;

namespace ui { // libboeyewidget.so

class BWidget : public QWidget {
public:

};

class BoeyeLine: public QWidget {
public:
  enum LineDirection { Vertical = 1, Horizontal = 2 };

  BoeyeLine(int weight, LineDirection dir, QWidget* parent);

};

class LineEdit : public QLineEdit {
public:
  enum BoeyeImType { im1, imDigits, imAlNum, im4, im5, im6 };
  LineEdit(QWidget * parent = 0);
  void flush();
};

class Dialog : public QDialog {
public:
  void InputDialog(QPixmap const& pixmap, QString const& title, QString const&, ui::LineEdit::BoeyeImType, QWidget* parent);
};

class InputDialog : public Dialog {
public:
  void inputtext();
};

}
#endif
