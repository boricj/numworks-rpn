#include "rpn_prompt_controller.h"
#include "app.h"
#include "../i18n.h"
#include <assert.h>

namespace Rpn {

RpnPromptController::ContentView::ContentView(Responder * parentResponder, RpnStackController * stackController, TextFieldDelegate * delegate) :
  View(),
  m_mainView(stackController, stackController, stackController),
  m_promptView(parentResponder, m_textBuffer, m_textBuffer, sizeof(m_textBuffer), delegate, false, KDText::FontSize::Large),
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
  Poincare::DynamicHierarchy * dynHier = nullptr;
  Poincare::StaticHierarchy<1> * staticHier1 = nullptr;
  Poincare::StaticHierarchy<2> * staticHier2 = nullptr;

  if (!(event == Ion::Events::Plus || event == Ion::Events::Minus || event == Ion::Events::Multiplication || event == Ion::Events::Division || event == Ion::Events::Space ||
        event == Ion::Events::Sine || event == Ion::Events::Cosine || event == Ion::Events::Tangent ||
        event == Ion::Events::Arcsine || event == Ion::Events::Arccosine || event == Ion::Events::Arctangent ||
        event == Ion::Events::Ln || event == Ion::Events::Log || event == Ion::Events::Exp ||
        event == Ion::Events::Sqrt || event == Ion::Events::Square || event == Ion::Events::Power)) {
    return false;
  }

  if (!pushInput()) {
    return false;
  }

  // Basic
  if (event == Ion::Events::Plus) {
    dynHier = new Poincare::Addition();
  }
  else if (event == Ion::Events::Minus) {
    staticHier2 = new Poincare::Subtraction();
  }
  else if (event == Ion::Events::Multiplication) {
    dynHier = new Poincare::Multiplication();
  }
  else if (event == Ion::Events::Division) {
    staticHier2 = new Poincare::Division();
  }
  else if (event == Ion::Events::Space) {
    staticHier1 = new Poincare::Opposite();
  }
  // Trigonometry
  else if (event == Ion::Events::Sine) {
    staticHier1 = new Poincare::Sine();
  }
  else if (event == Ion::Events::Cosine) {
    staticHier1 = new Poincare::Cosine();
  }
  else if (event == Ion::Events::Tangent) {
    staticHier1 = new Poincare::Tangent();
  }
  else if (event == Ion::Events::Arcsine) {
    staticHier1 = new Poincare::ArcSine();
  }
  else if (event == Ion::Events::Arccosine) {
    staticHier1 = new Poincare::ArcCosine();
  }
  else if (event == Ion::Events::Arctangent) {
    staticHier1 = new Poincare::ArcTangent();
  }
  // Logarithms
  else if (event == Ion::Events::Ln) {
    staticHier1 = new Poincare::NaperianLogarithm();
  }
  else if (event == Ion::Events::Log) {
    m_rpnStack->push(new Poincare::Rational(10));
    staticHier2 = new Poincare::Logarithm();
  }
  else if (event == Ion::Events::Exp) {
    m_rpnStack->push(new Poincare::Symbol(Ion::Charset::Exponential));
    m_rpnStack->swap();
    staticHier2 = new Poincare::Power();
  }
  // Square/Power
  else if (event == Ion::Events::Sqrt) {
    staticHier1 = new Poincare::SquareRoot();
  }
  else if (event == Ion::Events::Square) {
    m_rpnStack->push(new Poincare::Rational(2));
    staticHier2 = new Poincare::Power();
  }
  else if (event == Ion::Events::Power) {
    staticHier2 = new Poincare::Power();
  }

  if (dynHier) {
    m_rpnStack->doOperation(dynHier, ((Rpn::App*)app())->localContext());
  }
  else if (staticHier1) {
    m_rpnStack->doOperation(staticHier1, ((Rpn::App*)app())->localContext());
  }
  else if (staticHier2) {
    m_rpnStack->doOperation(staticHier2, ((Rpn::App*)app())->localContext());
  }
  else {
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
