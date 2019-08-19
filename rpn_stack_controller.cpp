
#include "rpn_stack_controller.h"
#include "app.h"
#include "rpn_prompt_controller.h"
#include "../shared/poincare_helpers.h"
#include <assert.h>
#include <poincare_nodes.h>

using namespace Poincare;

namespace Rpn {

RpnStackController::RpnStackController(Responder * parentResponder, RpnStack * rpnStack, SelectableTableView * view, RpnPromptController * promptController) :
  ViewController(parentResponder),
  m_rpnStack(rpnStack),
  m_view(view),
  m_promptController(promptController)
{
}

View * RpnStackController::view() {
  return m_view;
}

void RpnStackController::didBecomeFirstResponder() {
  reloadAndScroll();
  m_view->selectCellAtLocation(0, m_rpnStack->length() - 1);
}

bool RpnStackController::handleEvent(Ion::Events::Event event) {
  int stackRow = m_rpnStack->length() - selectedRow() - 1;
  if (event == Ion::Events::Down) {
    m_view->deselectTable();
    Container::activeApp()->setFirstResponder(m_promptController);
    return true;
  }
  else if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    m_view->deselectTable();
    m_promptController->setText((*m_rpnStack)[stackRow]);
    Container::activeApp()->setFirstResponder(m_promptController);
    return true;
  }
  else if (event == Ion::Events::Clear) {
    while (m_rpnStack->length() > selectedRow()+1) {
      pop();
    }
    return true;
  }
  else if (event == Ion::Events::Backspace/* || event == Ion::Events::Cut*/) {
    char backup[stackRow+1][RpnStack::k_expressionSize];
    for (int i = 0; i < stackRow+1; i++) {
      strlcpy(backup[i], (*m_rpnStack)[0], RpnStack::k_expressionSize);
      pop();
    }
    #if 0
    // FIXME: escher's SelectableTableView grab Ion::Events::Cut
    if (event == Ion::Events::Cut) {
      Clipboard::sharedClipboard()->store(backup[stackRow]);
    }
    #endif
    for (int i = stackRow - 1; i >= 0; i--) {
      push(backup[i], *((Rpn::App*) Container::activeApp())->localContext());
    }
    if (empty()) {
      Container::activeApp()->setFirstResponder(m_promptController);
    }
    else {
      reloadAndScroll();
      stackRow = m_rpnStack->length() - stackRow - 1;
      if (stackRow < 0) {
        stackRow = 0;
      }
      m_view->selectCellAtLocation(0, stackRow);
    }
    return true;
  }

  return false;
}

KDCoordinate RpnStackController::rowHeight(int i) {
  return m_rpnStack->height(m_rpnStack->length() - i - 1) + k_padding * 2;
}

void RpnStackController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  EvenOddExpressionCell *realCell = static_cast<EvenOddExpressionCell *>(cell);
  realCell->setEven(index%2);
  realCell->setAlignment(1, 0.5);

  realCell->setLayout(createLayout(index));
  realCell->reloadCell();
}

Poincare::Layout RpnStackController::createLayout(int index) {
  Expression e = m_rpnStack->expression(m_rpnStack->length() - index - 1);
  return e.createLayout(Preferences::sharedPreferences()->displayMode(), Preferences::sharedPreferences()->numberOfSignificantDigits());
}

void RpnStackController::dup() {
  if (!isFull()) {
    m_rpnStack->dup();
    reloadAndScroll();
  }
}

void RpnStackController::swap() {
  m_rpnStack->swap();
  reloadAndScroll();
}

void RpnStackController::rot() {
  m_rpnStack->rot();
  reloadAndScroll();
}

void RpnStackController::over() {
  if (!isFull()) {
    m_rpnStack->over();
    reloadAndScroll();
  }
}

bool RpnStackController::push(const char *text, Poincare::Context &context) {
  if (isFull()) {
    return false;
  }
  if (!m_rpnStack->push(text, context)) {
    Container::activeApp()->displayWarning(I18n::Message::SyntaxError);
    return false;
  }
  reloadAndScroll();
  return true;
}

void RpnStackController::push(Poincare::Expression &exp, Poincare::Context &context) {
  m_rpnStack->push(exp, context);
  m_view->deselectTable();
  reloadAndScroll();
}

void RpnStackController::pop() {
  m_rpnStack->pop();
  for (int i = 0; i < RpnStack::k_stackSize; i++) {
    m_cells[i].setHighlighted(false);
  }
  reloadAndScroll();
}

void RpnStackController::clear() {
  m_rpnStack->clear();
  for (int i = 0; i < RpnStack::k_stackSize; i++) {
    m_cells[i].setHighlighted(false);
  }
  m_view->deselectTable();
  reloadAndScroll();
}

bool RpnStackController::isFull() {
  if (m_rpnStack->full()) {
    Container::activeApp()->displayWarning(I18n::Message::StorageMemoryFull1, I18n::Message::StorageMemoryFull2, true);
    return true;
  }
  return false;
}

void RpnStackController::reloadAndScroll(int index) {
  if (index < 0 || index >= m_rpnStack->length()) {
    index = m_rpnStack->length()-1;
  }
  m_view->reloadData(false);
  m_view->scrollToCell(0, m_rpnStack->length()-1);
}

}
