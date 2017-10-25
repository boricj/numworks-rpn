#ifndef HELLOWORLD_HELLO_CONTROLLER_H
#define HELLOWORLD_HELLO_CONTROLLER_H

#include <escher.h>

#include "hello_view.h"

namespace Helloworld {

class HelloController : public ViewController {
public:
  HelloController(Responder * parentResponder);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
private:
  HelloView m_helloView;
};

}

#endif
