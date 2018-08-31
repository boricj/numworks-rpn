#ifndef RPN_RPN_CONTROLLER_H
#define RPN_RPN_CONTROLLER_H

#include <escher.h>

#include "rpn_prompt_view.h"

namespace Rpn {

class RpnPromptController : public ViewController {
public:
  RpnPromptController(Responder * parentResponder);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
private:
  static constexpr int k_bufferLength = TextField::maxBufferSize();
  char m_textBody[k_bufferLength];
  char *m_curTextPtr;
  RpnPromptView m_view;
};

}

#endif
