#include "rpn_toolbox.h"

#include "rpn_input_controller.h"
#include "rpn_stack_controller.h"

namespace Rpn {

Toolbox::Toolbox(const ToolboxMessageTree * rootModel, InputController * inputController, StackController * stackController) :
  ::Toolbox(nullptr, rootModel->label()),
  m_inputController(inputController),
  m_stackController(stackController),
  m_rootModel(rootModel)
{
}

bool Toolbox::selectLeaf(int selectedRow) {
  ToolboxMessageTree * messageTree = (ToolboxMessageTree *)m_messageTreeModel->children(selectedRow);
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

const ToolboxMessageTree * Toolbox::rootModel() const {
  return m_rootModel;
}

MessageTableCellWithMessage * Toolbox::leafCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

MessageTableCellWithChevron* Toolbox::nodeCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_nodeCells[index];
}

int Toolbox::maxNumberOfDisplayedRows() {
 return k_maxNumberOfDisplayedRows;
}

}
