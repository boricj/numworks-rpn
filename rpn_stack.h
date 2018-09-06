#ifndef RPN_STACK_H
#define RPN_STACK_H

#include "poincare.h"
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
  void pop();
  void clear();

  void logTen(Poincare::Context &context);
  void square(Poincare::Context &context);
  void exponentE(Poincare::Context &context);

  void doOperation(Poincare::DynamicHierarchy * exp, Poincare::Context &context);
  void doOperation(Poincare::StaticHierarchy<1> * exp, Poincare::Context &context);
  void doOperation(Poincare::StaticHierarchy<2> * exp, Poincare::Context &context);

  static constexpr int k_stackSize = 5;
private:
  void push(Poincare::Expression * exp);
  Poincare::Expression * m_stack[k_stackSize];
};

}

#endif
