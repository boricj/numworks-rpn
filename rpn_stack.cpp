#include "rpn_stack.h"
#include "../shared/poincare_helpers.h"
#include "ion/charset.h"
#include <string.h>
#include <poincare_nodes.h>

namespace Rpn {

RpnStack::RpnStack() {
  for (int i = 0; i < k_stackSize; i++) {
    strlcpy(m_expressions[i], "0", k_printedExpressionSize);
  }
  m_isPacked = true;
}

RpnStack::~RpnStack() {
  if (!m_isPacked) {
    for (int i = 0; i < k_stackSize; i++) {
      delete m_stack[i];
    }
  }
}

void RpnStack::dup() {
  assert(!m_isPacked);

  push(new Poincare::Expression((*this)[0].clone()));
}

void RpnStack::swap() {
  assert(!m_isPacked);

  Poincare::Expression * a = new Poincare::Expression((*this)[0].clone());
  Poincare::Expression * b = new Poincare::Expression((*this)[1].clone());
  pop();
  pop();
  push(a);
  push(b);
}

void RpnStack::rot() {
  assert(!m_isPacked);

  Poincare::Expression * a = new Poincare::Expression((*this)[0].clone());
  Poincare::Expression * b = new Poincare::Expression((*this)[1].clone());
  Poincare::Expression * c = new Poincare::Expression((*this)[2].clone());
  pop();
  pop();
  pop();
  push(b);
  push(a);
  push(c);
}

void RpnStack::over() {
  assert(!m_isPacked);

  push(new Poincare::Expression((*this)[1].clone()));
}

bool RpnStack::push(const char *text) {
  assert(!m_isPacked);

  Poincare::Expression * exp = new Poincare::Expression(Poincare::Expression::parse(text));
  if (exp == nullptr) {
    return false;
  }
  push(exp);
  return true;
}

void RpnStack::push(Poincare::Expression * exp) {
  assert(!m_isPacked);

  delete m_stack[k_stackSize-1];
  for (int i = k_stackSize-1; i > 0; i--) {
    m_stack[i] = m_stack[i-1];
  }
  m_stack[0] = exp;
  m_length += m_length < k_stackSize ? 1 : 0;
}


void RpnStack::pop() {
  assert(!m_isPacked);

  delete m_stack[0];
  for (int i = 0; i < k_stackSize-1; i++) {
    m_stack[i] = m_stack[i+1];
  }
  m_stack[k_stackSize-1] = new Poincare::Rational(0);
  m_length -= m_length > 0 ? 1 : 0;
}

void RpnStack::clear() {
  if (m_isPacked) {
    for (int i = 0; i < RpnStack::k_stackSize; i++) {
      m_expressions[i][0] = '\0';
    }
  }
  else {
    for (int i = 0; i < RpnStack::k_stackSize; i++) {
      this->pop();
    }
  }
}

void RpnStack::tidy() {
  if (!m_isPacked) {
    for (int i = 0; i < RpnStack::k_stackSize; i++) {
      m_stack[i]->serialize(m_expressions[i], sizeof(m_expressions[i]));
      delete m_stack[i];
    }

    m_isPacked = true;
  }
}

void RpnStack::unpack() {
  if (m_isPacked) {
    for (int i = 0; i < RpnStack::k_stackSize; i++) {
      m_stack[i] = new Poincare::Expression(Poincare::Expression::parse(m_expressions[i]));
    }

    m_isPacked = false;
  }
}

}
