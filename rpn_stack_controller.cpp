#include "rpn_stack_controller.h"
#include "../i18n.h"
#include <assert.h>

namespace Rpn {

RpnStackController::RpnStackController(Responder * parentResponder, RpnStack * rpnStack, View * view) :
  ViewController(parentResponder),
  m_rpnStack(rpnStack),
  m_view(view)
{
}

View * RpnStackController::view() {
  return m_view;
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
