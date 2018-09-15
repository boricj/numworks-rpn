#include "rpn_stack_controller.h"
#include "rpn_prompt_controller.h"
#include "../i18n.h"
#include <assert.h>

namespace Rpn {

RpnStackController::RpnStackController(Responder * parentResponder, RpnStack * rpnStack, View * view, RpnPromptController * promptController) :
  ViewController(parentResponder),
  m_rpnStack(rpnStack),
  m_view(view),
  m_promptController(promptController)
{
}

View * RpnStackController::view() {
  return m_view;
}

bool RpnStackController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    app()->setFirstResponder(m_promptController);
    return true;
  }
  else if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    char buffer[256];
    (*m_rpnStack)[RpnStack::k_stackSize - selectedRow() - 1].writeTextInBuffer(buffer, sizeof(buffer), Poincare::PrintFloat::Mode::Decimal);
    m_promptController->setText(buffer);
    app()->setFirstResponder(m_promptController);
    return true;
  }

  return false;
}

void RpnStackController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  char buffer[256];

  EvenOddBufferTextCell *realCell = static_cast<EvenOddBufferTextCell *>(cell);
  realCell->setEven(index%2);
  realCell->setFontSize(KDText::FontSize::Large);
  (*m_rpnStack)[RpnStack::k_stackSize - index - 1].writeTextInBuffer(buffer, sizeof(buffer), Poincare::PrintFloat::Mode::Decimal);
  realCell->setText(buffer);
  realCell->reloadCell();
}

}
