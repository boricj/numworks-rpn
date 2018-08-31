#include "rpn_controller.h"
#include <assert.h>

namespace Rpn {

RpnController::RpnController(Responder * parentResponder) :
  ViewController(parentResponder)
{
}

View * RpnController::view() {
  return &m_rpnView;
}

void RpnController::didBecomeFirstResponder() {
}

bool RpnController::handleEvent(Ion::Events::Event event) {
  return false;
}

}
