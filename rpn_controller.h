#ifndef RPN_RPN_CONTROLLER_H
#define RPN_RPN_CONTROLLER_H

#include <escher.h>

#include "rpn_view.h"

namespace Rpn {

class RpnController : public ViewController {
public:
  RpnController(Responder * parentResponder);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
private:
  RpnView m_rpnView;
};

}

#endif
