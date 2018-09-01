#ifndef RPN_STACK_H
#define RPN_STACK_H

#include "poincare.h"
#include <stddef.h>

namespace Rpn {

class RpnStack {
public:
  RpnStack();
  ~RpnStack();

  bool isFull() const { return size() >= (k_stackHeight - 1); }
  bool empty() const { return size() == 0; }
  bool push(const char *text);
  void push(Poincare::Expression * exp);
  void pop();
  size_t size() const { return m_curStackPtr - m_stack; }
  bool doOperation(Poincare::DynamicHierarchy * exp, Poincare::Context &context);
  bool doOperation(Poincare::StaticHierarchy<2> * exp, Poincare::Context &context);
  const Poincare::Expression & operator[](size_t idx) const { return *(m_stack[idx]); }

private:
  static constexpr int k_stackHeight = 6;
  Poincare::Expression * m_stack[k_stackHeight];
  Poincare::Expression ** m_curStackPtr;
};

}

#endif
