#include "rpn_prompt_controller.h"
#include <assert.h>

namespace Rpn {

RpnPromptController::RpnPromptController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_textBody(""),
  m_curTextPtr(m_textBody)
{
}

View * RpnPromptController::view() {
  return &m_view;
}

void RpnPromptController::didBecomeFirstResponder() {
}

bool RpnPromptController::handleEvent(Ion::Events::Event event) {
  char addCharacter = 0;
  bool handled = false;

  if (event == Ion::Events::Backspace) {
      if (m_curTextPtr > m_textBody) {
        *--m_curTextPtr = '\0';
        handled = true;
      }
  }
  else if (event == Ion::Events::Clear) {
    *m_textBody = '\0';
    m_curTextPtr = m_textBody;
      handled = true;
  }
  else if (event == Ion::Events::Dot) {
    addCharacter = '.';
  }
  else if (event == Ion::Events::Zero) {
    addCharacter = '0';
  }
  else if (event == Ion::Events::Zero) {
    addCharacter = '0';
  }
  else if (event == Ion::Events::One) {
    addCharacter = '1';
  }
  else if (event == Ion::Events::Two) {
    addCharacter = '2';
  }
  else if (event == Ion::Events::Three) {
    addCharacter = '3';
  }
  else if (event == Ion::Events::Four) {
    addCharacter = '4';
  }
  else if (event == Ion::Events::Five) {
    addCharacter = '5';
  }
  else if (event == Ion::Events::Six) {
    addCharacter = '6';
  }
  else if (event == Ion::Events::Seven) {
    addCharacter = '7';
  }
  else if (event == Ion::Events::Eight) {
    addCharacter = '8';
  }
  else if (event == Ion::Events::Nine) {
    addCharacter = '9';
  }
  else if (event == Ion::Events::EE) {
    addCharacter = '\x8d';
  }

  if (addCharacter && (m_curTextPtr-m_textBody < k_bufferLength)) {
    *m_curTextPtr++ = addCharacter;
    *m_curTextPtr = '\0';
    handled = true;
  }

  if (handled) {
    m_view.setText(m_textBody);
  }
  return handled;
}

}
