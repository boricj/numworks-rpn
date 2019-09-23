#ifndef RPN_RPN_STACK_CONTROLLER_H
#define RPN_RPN_STACK_CONTROLLER_H

#include <escher.h>

#include "rpn_stack.h"

namespace Rpn {
  class RpnPromptController;

class RpnStackController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  RpnStackController(Responder * parentResponder, RpnStack * rpnStack, SelectableTableView * view, RpnPromptController * promptController);
  View * view() override;
  void didBecomeFirstResponder() override;

  bool handleEvent(Ion::Events::Event event) override;

  int numberOfRows() const override { return m_rpnStack->length(); }
  KDCoordinate rowHeight(int i) override;
  HighlightCell * reusableCell(int index, int type) override { return &m_cells[index]; }
  int reusableCellCount(int type) { return RpnStack::k_stackSize; };
  virtual int typeAtLocation(int i, int j) { return 0; }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

  const char* operator[](size_t idx) const { return (*m_rpnStack)[idx]; }
  void dup();
  void swap();
  void rot();
  void over();
  bool push(const char *text, Poincare::Context &context);
  void push(Poincare::Expression &exp, Poincare::Context &context);
  void pop();
  void clear();
  void erase(bool upTo);

  bool approximate() { return m_rpnStack->approximate; }
  void setApproximate(bool approx) { m_rpnStack->approximate = approx; }
  bool full() { return m_rpnStack->full(); }
  bool empty() { return m_rpnStack->length() == 0; }
  Poincare::Expression exact(size_t index) { return m_rpnStack->exact(index); }
  void reloadAndScroll(int index = -1);

  constexpr static int k_padding = 4;
private:
  bool isFull();

  Poincare::Layout createLayout(int index);
  RpnStack * m_rpnStack;
  SelectableTableView * m_view;
  RpnPromptController * m_promptController;
  EvenOddExpressionCell m_cells[RpnStack::k_stackSize];
};

}

#endif
