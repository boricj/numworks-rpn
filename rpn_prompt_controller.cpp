#include "rpn_prompt_controller.h"
#include "app.h"
#include "../i18n.h"
#include <assert.h>
#include <poincare_nodes.h>

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
  switch (index) {
    case 0:  return &m_mainView;
    default: return &m_promptView;
  }
}

void RpnPromptController::ContentView::layoutSubviews() {
  KDCoordinate inputViewFrameHeight = 32;
  KDRect mainViewFrame(0, 0, bounds().width(), bounds().height() - inputViewFrameHeight);
  m_mainView.setFrame(mainViewFrame);
  KDRect inputViewFrame(0, bounds().height() - inputViewFrameHeight, bounds().width(), inputViewFrameHeight);
  m_promptView.setLeftMargin(4);
  m_promptView.setFrame(inputViewFrame);
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
  m_stackController.reloadAndScroll();
}

bool RpnPromptController::handleEvent(Ion::Events::Event event) {
  return false;
}

bool RpnPromptController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::EXE || event == Ion::Events::OK;
}

bool RpnPromptController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (handleEventSpecial(event) || handleEventOperation(event)) {
    m_stackController.reloadAndScroll();
    return true;
  }

  return false;
}

bool RpnPromptController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  if (pushInput()) {
    m_stackController.reloadAndScroll();
    return false;
  }
  else {
    return false;
  }
}

bool RpnPromptController::textFieldDidAbortEditing(TextField * textField) {
  m_stackController.reloadAndScroll();
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
      m_stackController.clear();
    }
    handled = true;
  }
  else if (event == Ion::Events::Backspace && *text == '\0') {
    m_stackController.pop();
    handled = true;
  }
  else if (event == Ion::Events::EXE && *text == '\0') {
    m_stackController.dup();
    handled = true;
  }
  else if (event == Ion::Events::Ans) {
    m_stackController.over();
    handled = true;
  }
  else if (event == Ion::Events::RightParenthesis) {
    m_stackController.swap();
    handled = true;
  }
  else if (event == Ion::Events::LeftParenthesis) {
    m_stackController.rot();
    handled = true;
  }
  else if (event == Ion::Events::Equal) {
    m_stackController.setApproximate(!m_stackController.approximate());
    handled = true;
  }
  else if (event == Ion::Events::XNT) {
    textField->handleEventWithText("x");
    handled = true;
  }
  else if (event == Ion::Events::Up && !m_stackController.empty()) {
    m_view.promptView()->setEditing(false, false);
    app()->setFirstResponder(&m_stackController);
    handled = true;
  }

  return handled;
}

bool RpnPromptController::handleEventOperation(Ion::Events::Event event) {

  if (!(event == Ion::Events::Plus || event == Ion::Events::Minus || event == Ion::Events::Multiplication || event == Ion::Events::Division || event == Ion::Events::Space ||
        event == Ion::Events::Sine || event == Ion::Events::Cosine || event == Ion::Events::Tangent ||
        event == Ion::Events::Arcsine || event == Ion::Events::Arccosine || event == Ion::Events::Arctangent ||
        event == Ion::Events::Ln || event == Ion::Events::Log || event == Ion::Events::Exp ||
        event == Ion::Events::Sqrt || event == Ion::Events::Square || event == Ion::Events::Power)) {
    return false;
  }

  if (!pushInput())
    return true;

  /* binary */

  Expression *e = nullptr;
  int pop;

  if (event == Ion::Events::Plus) {
    e = new Addition(m_stackController.exact(1), m_stackController.exact(0));
    pop = 2;
  }
  else if (event == Ion::Events::Minus) {
    e = new Subtraction(m_stackController.exact(1), m_stackController.exact(0));
    pop = 2;
  }
  else if (event == Ion::Events::Multiplication) {
    e = new Multiplication(m_stackController.exact(1), m_stackController.exact(0));
    pop = 2;
  }
  else if (event == Ion::Events::Division) {
    e = new Division(m_stackController.exact(1), m_stackController.exact(0));
    pop = 2;
  }
  else if (event == Ion::Events::Power) {
    e = new Power(m_stackController.exact(1), m_stackController.exact(0));
    pop = 2;
  }

  /* unary */

  else if (event == Ion::Events::Space) {
    e = new Opposite(m_stackController.exact(0));
    pop = 1;
  }
  else if (event == Ion::Events::Sine) {
    e = new Sine(m_stackController.exact(0));
    pop = 1;
  }
  else if (event == Ion::Events::Cosine) {
    e = new Cosine(m_stackController.exact(0));
    pop = 1;
  }
  else if (event == Ion::Events::Tangent) {
    e = new Tangent(m_stackController.exact(0));
    pop = 1;
  }
  else if (event == Ion::Events::Arcsine) {
    e = new ArcSine(m_stackController.exact(0));
    pop = 1;
  }
  else if (event == Ion::Events::Arccosine) {
    e = new ArcCosine(m_stackController.exact(0));
    pop = 1;
  }
  else if (event == Ion::Events::Arctangent) {
    e = new ArcTangent(m_stackController.exact(0));
    pop = 1;
  }
  else if (event == Ion::Events::Ln) {
    e = new NaperianLogarithm(m_stackController.exact(0));
    pop = 1;
  }
  else if (event == Ion::Events::Log) {
    e = new Logarithm(m_stackController.exact(0));
    pop = 1;
  }
  else if (event == Ion::Events::Exp) {
    e = new Power(Symbol(Ion::Charset::Exponential), m_stackController.exact(0));
    pop = 1;
  }
  else if (event == Ion::Events::Sqrt) {
    e = new SquareRoot(m_stackController.exact(0));
    pop = 1;
  }
  else if (event == Ion::Events::Square) {
    e = new Power(m_stackController.exact(0), Rational(2));
    pop = 1;
  }
  else {
    return false;
  }

  while (pop--) {
    m_stackController.pop();
  }

  m_stackController.push(*e, *((Rpn::App*) app())->localContext());
  delete e;

  return true;
}

bool RpnPromptController::pushInput() {
  TextField *textField = m_view.promptView();
  const char *text = textField->text();

  if (*text == '\0') {
    return true;
  }

  if (m_stackController.push(text, *((Rpn::App*) app())->localContext())) {
    textField->setText("");
    textField->setCursorLocation(0);
    return true;
  }
  else {
    return false;
  }
}

void RpnPromptController::setText(const char *text) {
  TextField *textField = m_view.promptView();
  textField->setEditing(true);
  textField->setText(text);
}

}
