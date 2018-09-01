#ifndef RPN_STACK_H
#define RPN_STACK_H

#include "poincare.h"

namespace Rpn {

class RpnStack {
public:
  RpnStack();
  ~RpnStack();

  bool isFull() const { return size() >= (k_stackHeight - 1); }
  bool empty() const { return size() == 0; }
  bool push(const char *text);
  void pop();
  size_t size() const { return m_curStackPtr - m_stack; }
  const Poincare::Expression & operator[](size_t idx) const { return *(m_stack[idx]); }

private:
  static constexpr int k_stackHeight = 9;
  Poincare::Expression * m_stack[k_stackHeight];
  Poincare::Expression ** m_curStackPtr;
};

}

#endif
