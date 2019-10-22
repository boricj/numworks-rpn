#ifndef RPN_STACK_CONTROLLER_H
#define RPN_STACK_CONTROLLER_H

#include <escher.h>

#include "rpn_stack.h"

namespace Rpn {
  class ContentView;
  class InputController;

class StackController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  StackController(Responder * parentResponder, Stack * stack, InputController * inputController, ContentView * view, Poincare::Context *context);
  View * view() override;
  void didBecomeFirstResponder() override;

  bool handleEvent(Ion::Events::Event event) override;

  int numberOfRows() const override { return m_stack->length(); }
  KDCoordinate rowHeight(int i) override;
  HighlightCell * reusableCell(int index, int type) override { return &m_cells[index]; }
  int reusableCellCount(int type) { return Stack::k_stackSize; };
  virtual int typeAtLocation(int i, int j) { return 0; }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

  const char* operator[](size_t idx) const { return (*m_stack)[idx]; }
  Stack& stack() { return *m_stack; }
  void erase(bool upTo);

  bool approximate() { return m_stack->approximate; }
  void setApproximate(bool approx) { m_stack->approximate = approx; }
  void reloadAndScroll(int index = -1);

  bool empty() { return m_stack->empty(); }

  I18n::Message operator()(const char* text);
  I18n::Message operator()(Stack::StackOperation op);
  I18n::Message operator()(Stack::SpecialOperation op);
  I18n::Message operator()(Poincare::ExpressionNode::Type op);
  I18n::Message operator()(I18n::Message op);

  SelectableTableView* stackView();

  constexpr static int k_padding = 4;
private:
  bool isFull();
  Poincare::Layout createLayout(int index);

  Stack * m_stack;
  InputController * m_inputController;
  ContentView * m_view;
  Poincare::Context * m_context;
  EvenOddExpressionCell m_cells[Stack::k_stackSize];
};

}

#endif
