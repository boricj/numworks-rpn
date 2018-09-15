#ifndef RPN_RPN_STACK_CONTROLLER_H
#define RPN_RPN_STACK_CONTROLLER_H

#include <escher.h>

#include "rpn_stack.h"

namespace Rpn {
  class RpnPromptController;

class RpnStackController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  RpnStackController(Responder * parentResponder, RpnStack * rpnStack, View * view, RpnPromptController * promptController);
  View * view() override;

  bool handleEvent(Ion::Events::Event event) override;

  int numberOfRows() override { return RpnStack::k_stackSize; }
  KDCoordinate rowHeight(int i) override { return 32; }
  HighlightCell * reusableCell(int index, int type) override { return &m_cells[index]; }
  int reusableCellCount(int type) { return 8; };
  virtual int typeAtLocation(int i, int j) { return 0; }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

private:
  RpnStack * m_rpnStack;
  View * m_view;
  RpnPromptController * m_promptController;
  EvenOddBufferTextCell m_cells[8];
};

}

#endif
