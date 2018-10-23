#include "rpn_prompt_controller.h"
#include "app.h"
#include "../i18n.h"
#include <assert.h>

using namespace Poincare;

namespace Rpn {

RpnPromptController::ContentView::ContentView(Responder * parentResponder, RpnStackController * stackController, TextFieldDelegate * delegate) :
  View(),
  m_mainView(stackController, stackController, stackController),
  m_promptView(parentResponder, m_textBuffer, m_textBuffer, sizeof(m_textBuffer), delegate, false, KDFont::LargeFont),
  m_textBuffer("")
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
  m_promptView.setLeftMargin(4);
}

void RpnPromptController::ContentView::reload() {
  layoutSubviews();
  markRectAsDirty(bounds());
}

RpnPromptController::RpnPromptController(Responder * parentResponder, RpnStack * rpnStack) :
  ViewController(parentResponder),
  m_rpnStack(rpnStack),
  m_view(parentResponder, &m_stackController, this),
  m_stackController(parentResponder, rpnStack, m_view.mainView(), this)
{
}

View * RpnPromptController::view() {
  return &m_view;
}

void RpnPromptController::didBecomeFirstResponder() {
  app()->setFirstResponder(m_view.promptView());
  m_view.promptView()->setEditing(true, false);
  m_view.mainView()->scrollToCell(0, m_rpnStack->length()-1);
  m_view.mainView()->deselectTable();
}

void RpnPromptController::willResignFirstResponder() {
  m_view.promptView()->setEditing(false, false);
}

void RpnPromptController::viewWillAppear() {
  m_view.promptView()->setEditing(true);
  m_view.promptView()->setText("");
}

bool RpnPromptController::handleEvent(Ion::Events::Event event) {
  return false;
}

bool RpnPromptController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::EXE || event == Ion::Events::OK || event == Ion::Events::Up;
}

bool RpnPromptController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (handleEventSpecial(event)) {
    m_view.mainView()->reloadData(false);
    m_view.mainView()->scrollToCell(0, m_rpnStack->length()-1);
    return true;
  }
  else if (handleEventOperation(event)) {
    m_view.mainView()->reloadData(false);
    m_view.mainView()->scrollToCell(0, m_rpnStack->length()-1);
    return true;
  }

  return false;
}

bool RpnPromptController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    if (pushInput()) {
      m_view.mainView()->reloadData(false);
      m_view.mainView()->scrollToCell(0, m_rpnStack->length()-1);
      return false;
    }
    else {
      return false;
    }
  }
  else if (event == Ion::Events::Up && m_rpnStack->length()) {
    m_view.mainView()->selectCellAtLocation(0, m_rpnStack->length() - 1);
    app()->setFirstResponder(m_view.mainView());
  }
  return false;
}

bool RpnPromptController::textFieldDidAbortEditing(TextField * textField) {
  m_view.mainView()->selectCellAtLocation(0, m_rpnStack->length() - 1);
  app()->setFirstResponder(m_view.mainView());
  return true;
}

bool RpnPromptController::textFieldDidHandleEvent(TextField * textField, bool returnValue, bool textHasChanged) {
  return returnValue;
}

bool RpnPromptController::handleEventSpecial(Ion::Events::Event event) {
  TextField *textField = m_view.promptView();
  const char *text = textField->text();
  bool handled = false;

  if (event == Ion::Events::Clear) {
    if (textField->isEditing() && *text) {
      textField->setText("");
      textField->setCursorLocation(0);
    }
    else {
      m_rpnStack->clear();
    }
    handled = true;
  }
  else if (event == Ion::Events::Backspace && *text == '\0') {
    m_rpnStack->pop();
    handled = true;
  }
  else if (event == Ion::Events::EXE && *text == '\0') {
    m_rpnStack->dup();
    handled = true;
  }
  else if (event == Ion::Events::Ans) {
    m_rpnStack->over();
    handled = true;
  }
  else if (event == Ion::Events::RightParenthesis) {
    m_rpnStack->swap();
    handled = true;
  }
  else if (event == Ion::Events::LeftParenthesis) {
    m_rpnStack->rot();
    handled = true;
  }

  return handled;
}

bool RpnPromptController::handleEventOperation(Ion::Events::Event event) {

  if (!pushInput())
    return false;

  if (!m_rpnStack->length())
    return false;

  /* binary */

  if (event == Ion::Events::Plus) {
    if (m_rpnStack->length() < 2) return false;
    Expression *e = new Addition((*m_rpnStack)[1].clone(), (*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else if (event == Ion::Events::Minus) {
    if (m_rpnStack->length() < 2) return false;
    Expression *e = new Subtraction((*m_rpnStack)[1].clone(), (*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else if (event == Ion::Events::Multiplication) {
    if (m_rpnStack->length() < 2) return false;
    Expression *e = new Multiplication((*m_rpnStack)[1].clone(), (*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else if (event == Ion::Events::Division) {
    if (m_rpnStack->length() < 2) return false;
    Expression *e = new Division((*m_rpnStack)[1].clone(), (*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else if (event == Ion::Events::Power) {
    if (m_rpnStack->length() < 2) return false;
    Expression *e = new Power((*m_rpnStack)[1].clone(), (*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->pop();
    m_rpnStack->push(e);

  /* unary */

  } else if (event == Ion::Events::Space) {
    if (m_rpnStack->length() < 1) return false;
    Expression *e = new Opposite((*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else if (event == Ion::Events::Sine) {
    if (m_rpnStack->length() < 1) return false;
    Expression *e = new Sine((*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else if (event == Ion::Events::Cosine) {
    if (m_rpnStack->length() < 1) return false;
    Expression *e = new Cosine((*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else if (event == Ion::Events::Tangent) {
    if (m_rpnStack->length() < 1) return false;
    Expression *e = new Tangent((*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else if (event == Ion::Events::Arcsine) {
    if (m_rpnStack->length() < 1) return false;
    Expression *e = new ArcSine((*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else if (event == Ion::Events::Arccosine) {
    if (m_rpnStack->length() < 1) return false;
    Expression *e = new ArcCosine((*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else if (event == Ion::Events::Arctangent) {
    if (m_rpnStack->length() < 1) return false;
    Expression *e = new ArcTangent((*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else if (event == Ion::Events::Ln) {
    if (m_rpnStack->length() < 1) return false;
    Expression *e = new NaperianLogarithm((*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else if (event == Ion::Events::Log) {
    if (m_rpnStack->length() < 1) return false;
    Expression *e = new Logarithm((*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else if (event == Ion::Events::Exp) {
    if (m_rpnStack->length() < 1) return false;
    Expression *e = new Power(Symbol(Ion::Charset::Exponential), (*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else if (event == Ion::Events::Sqrt) {
    if (m_rpnStack->length() < 1) return false;
    Expression *e = new SquareRoot((*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else if (event == Ion::Events::Square) {
    if (m_rpnStack->length() < 1) return false;
    Expression *e = new Power(Rational(2), (*m_rpnStack)[0].clone());
    m_rpnStack->pop();
    m_rpnStack->push(e);

  } else {
    return false;
  }

  return true;
}

bool RpnPromptController::pushInput() {
  TextField *textField = m_view.promptView();
  const char *text = textField->text();

  if (*text == '\0') {
    return true;
  }

  if (m_rpnStack->push(text)) {
    textField->setText("");
    textField->setCursorLocation(0);
    return true;
  }
  else {
    app()->displayWarning(I18n::Message::SyntaxError);
    return false;
  }
}

void RpnPromptController::setText(const char *text) {
  TextField *textField = m_view.promptView();
  textField->setEditing(true);
  textField->setText(text);
}

}
