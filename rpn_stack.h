#ifndef RPN_STACK_H
#define RPN_STACK_H

#include <escher.h>
#include <poincare.h>
#include <stddef.h>

namespace Rpn {

class RpnStack {
public:
  RpnStack();
  ~RpnStack();

  const Poincare::Expression & operator[](size_t idx) const { return *(m_stack[idx]); }

  void dup();
  void swap();
  void rot();
  void over();
  bool push(const char *text);
  void push(Poincare::Expression * exp);
  void pop();
  void clear();

  void tidy();
  void unpack();

  int length() const { return m_length; }

  static constexpr int k_stackSize = 16;
  constexpr static int k_printedExpressionSize = 2*::TextField::maxBufferSize();
private:
  Poincare::Expression *m_stack[k_stackSize];
  char m_expressions[k_stackSize][k_printedExpressionSize];
  int m_length;
  bool m_isPacked;
};

}

#endif
