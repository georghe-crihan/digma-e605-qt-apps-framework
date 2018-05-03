#ifndef IMCONTEXT_HPP_INCLUDED
#define IMCONTEXT_HPP_INCLUDED

class QWSServer;

namespace sys {

class Imcontext : public QWSServer {
public:
  static void installInputMethod(void);
};

}

#endif
