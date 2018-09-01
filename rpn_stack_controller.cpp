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
  if ((size_t)index >= m_rpnStack->size()) {
    realCell->setText("");
  }
  else {
    (*m_rpnStack)[index].writeTextInBuffer(buffer, sizeof(buffer), Poincare::PrintFloat::Mode::Decimal);
    realCell->setText(buffer);
  }
  realCell->reloadCell();
}

}
