#ifndef RPN_TOOLBOX_H
#define RPN_TOOLBOX_H

#include <escher.h>
#include <apps/i18n.h>

namespace Rpn {

class InputController;
class StackController;

class Toolbox : public ::Toolbox {
public:
  Toolbox(const ToolboxMessageTree * rootModel, InputController * inputController, StackController * stackController);
protected:
  bool selectLeaf(int selectedRow) override;
  const ToolboxMessageTree * rootModel() const override;
  MessageTableCellWithMessage * leafCellAtIndex(int index) override;
  MessageTableCellWithChevron* nodeCellAtIndex(int index) override;
  int maxNumberOfDisplayedRows() override;
  constexpr static int k_maxNumberOfDisplayedRows = 6; // = 240/40
private:
  MessageTableCellWithMessage m_leafCells[k_maxNumberOfDisplayedRows];
  MessageTableCellWithChevron m_nodeCells[k_maxNumberOfDisplayedRows];
  InputController * m_inputController;
  StackController * m_stackController;
  const ToolboxMessageTree * m_rootModel;
};

}

#endif
