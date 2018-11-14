#ifndef RPN_STACK_H
#define RPN_STACK_H

#include <escher.h>
#include <poincare/expression.h>
#include <stddef.h>

namespace Rpn {

class RpnStack {
public:
  constexpr static int k_stackSize = 8;
  constexpr static int k_expressionSize = 2*::TextField::maxBufferSize();

private:
  struct RpnElement {
    RpnElement();
    RpnElement(Poincare::Expression &exp, Poincare::Context &context);

    char expression[k_expressionSize];
    char approximate[k_expressionSize];
    KDCoordinate expressionHeight;
    KDCoordinate approximateHeight;
  };

public:
  RpnStack() = default;

  const char* operator[](size_t idx) const { return approximate ? m_stack[idx].approximate : m_stack[idx].expression; }
  Poincare::Expression exact(size_t idx) const;
  Poincare::Expression expression(size_t idx) const;
  KDCoordinate height(size_t idx) const { return approximate ? m_stack[idx].approximateHeight : m_stack[idx].expressionHeight; }

  void dup();
  void swap();
  void rot();
  void over();
  bool push(const char *text, Poincare::Context &context);
  void push(Poincare::Expression &exp, Poincare::Context &context);
  void pop();
  void clear();

  int length() const { return m_length; }
  bool full() const { return m_length >= k_stackSize; }

  bool approximate;

private:
  void push(RpnElement &exp);

  RpnElement m_stack[k_stackSize];
  int m_length;
};

}

#endif
