#include "rpn_prompt_controller.h"
#include "app.h"
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
  KDRect mainViewFrame(0, 0, bounds().width(), bounds().height() - inputViewFrameHeight);
  m_mainView.setFrame(mainViewFrame);
  KDRect inputViewFrame(0, bounds().height() - inputViewFrameHeight, bounds().width(), inputViewFrameHeight);
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
  else if (handleDigit(event)) {
    handled = true;
  }
  else if (handleOperation(event)) {
    handled = true;
  }

  if (handled) {
    m_view.promptView()->setText(m_textBody);
  }
  return handled;
}

bool RpnPromptController::handleDigit(Ion::Events::Event event) {
  char addCharacter = '\0';
  if (event == Ion::Events::Dot) {
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

  if (addCharacter && (m_curTextPtr-m_textBody < k_bufferLength) && !m_rpnStack->isFull()) {
    *m_curTextPtr++ = addCharacter;
    *m_curTextPtr = '\0';
    return true;
  }

  return false;
}

bool RpnPromptController::handleOperation(Ion::Events::Event event) {
  bool handled = false;

  Poincare::DynamicHierarchy * dynHier = nullptr;
  Poincare::StaticHierarchy<2> * staticHier2 = nullptr;

  if (event == Ion::Events::Plus) {
    if (m_curTextPtr != m_textBody) {
      handleEventEXE();
    }
    dynHier = new Poincare::Addition();
  }
  else if (event == Ion::Events::Minus) {
    if (m_curTextPtr != m_textBody) {
      handleEventEXE();
    }
    staticHier2 = new Poincare::Subtraction();
  }
  else if (event == Ion::Events::Multiplication) {
    if (m_curTextPtr != m_textBody) {
      handleEventEXE();
    }
    dynHier = new Poincare::Multiplication();
  }
  else if (event == Ion::Events::Division) {
    if (m_curTextPtr != m_textBody) {
      handleEventEXE();
    }
    staticHier2 = new Poincare::Division();
  }

  if (dynHier) {
    m_rpnStack->doOperation(dynHier, ((Rpn::App*)app())->localContext());
    dynHier = nullptr;
    handled = true;
  }
  else if (staticHier2) {
    m_rpnStack->doOperation(staticHier2, ((Rpn::App*)app())->localContext());
    staticHier2 = nullptr;
    handled = true;
  }

  if (handled) {
    m_view.mainView()->reloadData(false);
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
