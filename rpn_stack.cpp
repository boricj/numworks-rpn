#include "rpn_stack.h"
#include "../shared/poincare_helpers.h"
#include "ion/charset.h"
#include <string.h>

using namespace Shared;

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
  push((*this)[0].clone());
}

void RpnStack::swap() {
  Poincare::Expression * a = (*this)[0].clone();
  Poincare::Expression * b = (*this)[1].clone();
  pop();
  pop();
  push(a);
  push(b);
}

void RpnStack::rot() {
  Poincare::Expression * a = (*this)[0].clone();
  Poincare::Expression * b = (*this)[1].clone();
  Poincare::Expression * c = (*this)[2].clone();
  pop();
  pop();
  pop();
  push(b);
  push(a);
  push(c);
}

void RpnStack::over() {
  push((*this)[1].clone());
}

bool RpnStack::push(const char *text) {
  Poincare::Expression * exp = Poincare::Expression::parse(text);
  if (exp == nullptr) {
    return false;
  }
  push(exp);
  return true;
}

void RpnStack::push(Poincare::Expression * exp) {
  delete m_stack[k_stackSize-1];
  for (int i = k_stackSize-1; i > 0; i--) {
    m_stack[i] = m_stack[i-1];
  }
  m_stack[0] = exp;
  m_length += m_length < k_stackSize ? 1 : 0;
}

void RpnStack::pop() {
  delete m_stack[0];
  for (int i = 0; i < k_stackSize-1; i++) {
    m_stack[i] = m_stack[i+1];
  }
  m_stack[k_stackSize-1] = new Poincare::Rational(0);
  m_length -= m_length > 0 ? 1 : 0;
}

void RpnStack::clear() {
  for (int i = 0; i < RpnStack::k_stackSize; i++) {
    this->pop();
  }
}

void RpnStack::doOperation(Poincare::DynamicHierarchy * exp, Poincare::Context *context) {
  exp->addOperand((*this)[1].clone());
  exp->addOperand((*this)[0].clone());
  pop();
  pop();

  push(PoincareHelpers::Approximate<double>(exp, *context));
  delete exp;
}

void RpnStack::doOperation(Poincare::StaticHierarchy<1> * exp, Poincare::Context *context) {
  Poincare::ListData listData;
  listData.pushExpression((*this)[0].clone());
  exp->setArgument(&listData, 2, true);
  pop();

  push(PoincareHelpers::Approximate<double>(exp, *context));
  delete exp;
}

void RpnStack::doOperation(Poincare::StaticHierarchy<2> * exp, Poincare::Context *context) {
  Poincare::ListData listData;
  listData.pushExpression((*this)[1].clone());
  listData.pushExpression((*this)[0].clone());
  exp->setArgument(&listData, 2, true);
  pop();
  pop();

  push(PoincareHelpers::Approximate<double>(exp, *context));
  delete exp;
}

}
