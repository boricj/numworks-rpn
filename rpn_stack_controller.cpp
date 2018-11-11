
#include "rpn_stack_controller.h"
#include "app.h"
#include "rpn_prompt_controller.h"
#include "../i18n.h"
#include "../shared/poincare_helpers.h"
#include <assert.h>

using namespace Poincare;

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
    m_promptController->setText((*m_rpnStack)[m_rpnStack->length() - selectedRow() - 1]);
    app()->setFirstResponder(m_promptController);
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

}
