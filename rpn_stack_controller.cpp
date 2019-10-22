#include "app.h"
#include "rpn_content_view.h"
#include "rpn_stack_controller.h"
#include "rpn_input_controller.h"
#include "../shared/poincare_helpers.h"
#include <assert.h>
#include <poincare_nodes.h>

using namespace Poincare;

namespace Rpn {

StackController::StackController(Responder * parentResponder, Stack * Stack, InputController * inputController, ContentView * view) :
  ViewController(parentResponder),
  m_stack(Stack),
  m_inputController(inputController),
  m_view(view)
{
}

View * StackController::view() {
  return m_view;
}

void StackController::didBecomeFirstResponder() {
  reloadAndScroll();
  stackView()->selectCellAtLocation(0, m_stack->length() - 1);
}

bool StackController::handleEvent(Ion::Events::Event event) {
  int stackRow = m_stack->length() - selectedRow() - 1;
  if (event == Ion::Events::Down || event == Ion::Events::Back) {
    stackView()->deselectTable();
    Container::activeApp()->setFirstResponder(m_inputController);
    return true;
  }

  return false;
}

KDCoordinate StackController::rowHeight(int i) {
  return m_stack->height(m_stack->length() - i - 1) + k_padding * 2;
}

void StackController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  EvenOddExpressionCell *realCell = static_cast<EvenOddExpressionCell *>(cell);
  realCell->setEven(index%2);
  realCell->setAlignment(1, 0.5);

  realCell->setLayout(createLayout(index));
  realCell->reloadCell();
}

Poincare::Layout StackController::createLayout(int index) {
  Expression e = Expression::Parse((*m_stack)[m_stack->length() - index - 1]);
  return e.createLayout(Preferences::sharedPreferences()->displayMode(), Preferences::sharedPreferences()->numberOfSignificantDigits());
}

void StackController::reloadAndScroll(int index) {
  if (index < 0 || index >= m_stack->length()) {
    index = m_stack->length()-1;
  }
  stackView()->reloadData(false);
  stackView()->scrollToCell(0, m_stack->length()-1);
}

SelectableTableView* StackController::stackView() {
  return m_view->stackView();
}


I18n::Message StackController::operator()(const char* text, Poincare::Context *context) {
  auto r = (*m_stack)(text, context);
  if (r != I18n::Message::Default) {
    reloadAndScroll();
  }
  return r;
}

I18n::Message StackController::operator()(Stack::StackOperation op) {
  auto r = (*m_stack)(op);
  if (r != I18n::Message::Default) {
    reloadAndScroll();
  }
  return r;
}

I18n::Message StackController::operator()(Stack::SpecialOperation op, Poincare::Context *context) {
  auto r = (*m_stack)(op, context);
  if (r != I18n::Message::Default) {
    reloadAndScroll();
  }
  return r;
}

I18n::Message StackController::operator()(Poincare::ExpressionNode::Type op, Poincare::Context *context) {
  auto r = (*m_stack)(op, context);
  if (r != I18n::Message::Default) {
    reloadAndScroll();
  }
  return r;
}

}
