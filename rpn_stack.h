#ifndef RPN_STACK_H
#define RPN_STACK_H

#include "poincare.h"
#include <stddef.h>

namespace Rpn {

class RpnStack {
public:
  RpnStack();
  ~RpnStack();

  bool full() const { return size() >= k_stackHeight; }
  bool empty() const { return size() == 0; }
  size_t size() const { return m_curStackPtr - m_stack; }
  const Poincare::Expression & operator[](size_t idx) const { return *(m_stack[idx]); }

  void dup();
  void swap();
  void rot();
  void over();
  bool push(const char *text);
  void pop();

  bool doOperation(Poincare::DynamicHierarchy * exp, Poincare::Context &context);
  bool doOperation(Poincare::StaticHierarchy<1> * exp, Poincare::Context &context);
  bool doOperation(Poincare::StaticHierarchy<2> * exp, Poincare::Context &context);

private:
  void push(Poincare::Expression * exp);
  static constexpr int k_stackHeight = 5;
  Poincare::Expression * m_stack[k_stackHeight];
  Poincare::Expression ** m_curStackPtr;
};

}

#endif
