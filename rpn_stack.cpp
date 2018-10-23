#include "rpn_stack.h"
#include "../shared/poincare_helpers.h"
#include "ion/charset.h"
#include <string.h>

namespace Rpn {

RpnStack::RpnStack() {
  for (int i = 0; i < k_stackSize; i++) {
    m_stack[i] = new Poincare::Rational(0);
  }
}

RpnStack::~RpnStack() {
  for (int i = 0; i < k_stackSize; i++) {
    delete m_stack[i];
  }
}

void RpnStack::dup() {
  push(new Poincare::Expression((*this)[0].clone()));
}

void RpnStack::swap() {
  if (m_length < 2)
      return;

  Poincare::Expression *tmp = m_stack[m_length - 1];
  m_stack[m_length - 1] = m_stack[m_length - 2];
  m_stack[m_length - 2] = tmp;
}

void RpnStack::rot() {
  if (m_length < 2)
      return;

  // move everything one element forward
  Poincare::Expression *tmp, *last = m_stack[0];
  for (int i = 1; i < m_length; i++) {
      tmp = m_stack[i];
      m_stack[i] = last;
      last = tmp;
  }
  m_stack[0] = last;
}

void RpnStack::over() {
  push(new Poincare::Expression((*this)[1].clone()));
}

bool RpnStack::push(const char *text) {
  Poincare::Expression *exp = new Poincare::Expression(Poincare::Expression::parse(text));
  if (exp == nullptr) {
    return false;
  }
  push(exp);
  return true;
}

void RpnStack::push(Poincare::Expression * exp) {
  if (m_length == k_stackSize)
      return;

  delete m_stack[m_length];
  m_stack[m_length] = exp;
  m_length++;
}

Poincare::Expression RpnStack::pop() {
  if (!m_length)
      return Poincare::Rational(0);

  m_length--;

  Poincare::Expression e = *m_stack[m_length];
  delete m_stack[m_length];
  m_stack[m_length] = new Poincare::Rational(0);

  return e;
}

void RpnStack::clear() {
  for (int i = 0; i < RpnStack::k_stackSize; i++) {
      delete m_stack[i];
      m_stack[i] = new Poincare::Rational(0);
  }
  m_length = 0;
}

}
