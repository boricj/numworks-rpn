#include "rpn_toolbox.h"

#include "rpn_input_controller.h"
#include "rpn_stack_controller.h"

namespace Rpn {

Toolbox::Toolbox(InputController * inputController, StackController * stackController) :
  m_inputController(inputController),
  m_stackController(stackController)
{
}

bool Toolbox::selectLeaf(int selectedRow) {
  ToolboxMessageTree * messageTree = (ToolboxMessageTree *)m_messageTreeModel->childAtIndex(selectedRow);
  m_selectableTableView.deselectTable();

  I18n::Message r = I18n::Message::Default;
  if (m_inputController->pushInput()) {
    r = (*m_stackController)(messageTree->label());
  }
  Container::activeApp()->dismissModalViewController();
  if (r != I18n::Message::Default) {
    Container::activeApp()->displayWarning(r);
  }
  return true;
}

}
