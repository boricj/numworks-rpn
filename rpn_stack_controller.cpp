
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
    char buffer[256];
    (*m_rpnStack)[m_rpnStack->length() - selectedRow() - 1].serialize(buffer, sizeof(buffer));
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
  realCell->setFont(KDFont::LargeFont);

  const Expression *e = &(*m_rpnStack)[m_rpnStack->length() - index - 1];
  Expression approx = e->approximate<double>(
          *((Rpn::App*) app())->localContext(),
          Preferences::sharedPreferences()->angleUnit(),
          Preferences::sharedPreferences()->complexFormat()
  );

  approx.serialize(buffer, sizeof(buffer));

  realCell->setText(buffer);
  realCell->reloadCell();
}

}
