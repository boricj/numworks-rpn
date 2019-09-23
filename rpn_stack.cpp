#include "rpn_stack.h"
#include "../shared/poincare_helpers.h"
#include <string.h>
#include <poincare_nodes.h>
#include <poincare/preferences.h>

namespace Rpn {

RpnStack::RpnElement::RpnElement() : expression("0"), approximate("0"), expressionHeight(KDFont::LargeFont->glyphSize().height()), approximateHeight(KDFont::LargeFont->glyphSize().height()) {
}

RpnStack::RpnElement::RpnElement(Poincare::Expression &exp, Poincare::Context &context) {
  Poincare::Expression exact = exp.simplify(&context, Poincare::Preferences::sharedPreferences()->complexFormat(), Poincare::Preferences::sharedPreferences()->angleUnit());
  exact.serialize(expression, k_expressionSize);
  expressionHeight = exact.createLayout(
    Poincare::Preferences::sharedPreferences()->displayMode(),
    Poincare::Preferences::sharedPreferences()->numberOfSignificantDigits()
  ).layoutSize().height();

  Poincare::Expression approx = exact.approximate<double>(
    &context,
    Poincare::Preferences::sharedPreferences()->complexFormat(),
    Poincare::Preferences::sharedPreferences()->angleUnit()
  );
  approx.serialize(approximate, k_expressionSize);
  approximateHeight = approx.createLayout(
    Poincare::Preferences::sharedPreferences()->displayMode(),
    Poincare::Preferences::sharedPreferences()->numberOfSignificantDigits()
  ).layoutSize().height();
}

Poincare::Expression RpnStack::exact(size_t idx) const {
  return Poincare::Expression::Parse(m_stack[idx].expression);
}

Poincare::Expression RpnStack::expression(size_t idx) const {
  return Poincare::Expression::Parse((*this)[idx]);
}

void RpnStack::dup() {
  RpnElement a = m_stack[0];
  push(a);
}

void RpnStack::swap() {
  RpnElement a = m_stack[0];
  RpnElement b = m_stack[1];
  pop();
  pop();
  push(a);
  push(b);
}

void RpnStack::rot() {
  RpnElement a = m_stack[0];
  RpnElement b = m_stack[1];
  RpnElement c = m_stack[2];
  pop();
  pop();
  pop();
  push(b);
  push(a);
  push(c);
}

void RpnStack::over() {
  RpnElement a = m_stack[1];
  push(a);
}

bool RpnStack::push(const char *text, Poincare::Context &context) {
  Poincare::Expression exp = Poincare::Expression::Parse(text);
  if (exp.isUninitialized()) {
    return false;
  }
  push(exp, context);
  return true;
}

void RpnStack::push(Poincare::Expression &e, Poincare::Context &context) {
  RpnElement a(e, context);
  push(a);
}

void RpnStack::push(RpnElement &e) {
  for (int i = k_stackSize-1; i > 0; i--) {
    m_stack[i] = m_stack[i-1];
  }

  m_stack[0] = e;
  m_length += m_length < k_stackSize ? 1 : 0;
}

void RpnStack::pop() {
  for (int i = 0; i < k_stackSize-1; i++) {
    m_stack[i] = m_stack[i+1];
  }
  m_stack[k_stackSize-1] = RpnElement();
  m_length -= m_length > 0 ? 1 : 0;
}

void RpnStack::clear() {
  while (m_length) {
    pop();
  }
}

}
