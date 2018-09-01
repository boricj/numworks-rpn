#include "rpn_prompt_controller.h"
#include "../i18n.h"
#include <assert.h>

namespace Rpn {

RpnPromptController::ContentView::ContentView(Responder * parentResponder, RpnStackController * stackController) :
  View(),
  m_mainView(stackController, stackController, stackController),
  m_promptView(KDText::FontSize::Large, 1.0f, 0.5f)
{
}

RpnPromptController::ContentView::~ContentView() {
}

View * RpnPromptController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  if (index == 0) {
    return &m_mainView;
  }
  assert(index == 1);
  return &m_promptView;
}

void RpnPromptController::ContentView::layoutSubviews() {
  KDCoordinate inputViewFrameHeight = 32;
  KDRect mainViewFrame(0, inputViewFrameHeight, bounds().width(), bounds().height() - inputViewFrameHeight);
  m_mainView.setFrame(mainViewFrame);
  KDRect inputViewFrame(0, 0, bounds().width(), inputViewFrameHeight);
  m_promptView.setFrame(inputViewFrame);
}

void RpnPromptController::ContentView::reload() {
  layoutSubviews();
  markRectAsDirty(bounds());
}

RpnPromptController::RpnPromptController(Responder * parentResponder, RpnStack * rpnStack) :
  ViewController(parentResponder),
  m_textBody(""),
  m_curTextPtr(m_textBody),
  m_rpnStack(rpnStack),
  m_view(parentResponder, &m_stackController),
  m_stackController(parentResponder, rpnStack, m_view.mainView())
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
    }
    else {
      if (!m_rpnStack->empty()) {
        m_rpnStack->pop();
      }
      m_view.mainView()->reloadData(false);
    }
    handled = true;
  }
  else if (event == Ion::Events::Clear) {
    if (m_curTextPtr > m_textBody) {
      *m_textBody = '\0';
      m_curTextPtr = m_textBody;
    }
    else {
      while (!m_rpnStack->empty()) {
        m_rpnStack->pop();
      }
      m_view.mainView()->reloadData(false);
    }
    handled = true;
  }
  else if (event == Ion::Events::EXE) {
    handleEventEXE();
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
    m_view.promptView()->setText(m_textBody);
  }
  return handled;
}

void RpnPromptController::handleEventEXE() {
  if (m_rpnStack->isFull()) {
    return;
  }

  if (m_curTextPtr > m_textBody) {
    if (m_rpnStack->push(m_textBody)) {
      *m_textBody = '\0';
      m_curTextPtr = m_textBody;
      m_view.mainView()->reloadData(false);
    }
    else {
      app()->displayWarning(I18n::Message::SyntaxError);
    }
  }
}

}
