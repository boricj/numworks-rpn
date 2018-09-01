#include "rpn_stack.h"
#include "../shared/poincare_helpers.h"
#include <string.h>

using namespace Shared;

namespace Rpn {

RpnStack::RpnStack() {
  memset(m_stack, 0, sizeof(m_stack));
  m_curStackPtr = m_stack;
}

RpnStack::~RpnStack() {
  while (!empty()) {
    pop();
  }
}

void RpnStack::dup() {
  if (!empty() && !full()) { 
    push((*this)[size()-1].clone());
  }
}

void RpnStack::swap() {
  if (size() >= 2) {
    Poincare::Expression * a = (*this)[size()-1].clone();
    Poincare::Expression * b = (*this)[size()-2].clone();
    pop();
    pop();
    push(a);
    push(b);
  }
}

void RpnStack::rot() {
  if (size() >= 3) {
    Poincare::Expression * a = (*this)[size()-1].clone();
    Poincare::Expression * b = (*this)[size()-2].clone();
    Poincare::Expression * c = (*this)[size()-3].clone();
    pop();
    pop();
    pop();
    push(b);
    push(a);
    push(c);
  }
}

void RpnStack::over() {
  if ((size() >= 2) && !full()) {
    push((*this)[size()-2].clone());
  }
}

bool RpnStack::push(const char *text) {
  assert(!full());
  Poincare::Expression * exp = Poincare::Expression::parse(text);
  if (exp == nullptr) {
    return false;
  }
  *m_curStackPtr++ = exp;
  return true;
}

void RpnStack::push(Poincare::Expression * exp) {
  assert(!full());
  *m_curStackPtr++ = exp;
}

void RpnStack::pop() {
  assert(!empty());
  --m_curStackPtr;
  delete *m_curStackPtr;
  *m_curStackPtr = nullptr;
}

bool RpnStack::doOperation(Poincare::DynamicHierarchy * exp, Poincare::Context &context) {
  if (size() < 2) {
    delete exp;
    return false;
  }

  exp->addOperand((*this)[size()-2].clone());
  exp->addOperand((*this)[size()-1].clone());
  pop();
  pop();

  push(PoincareHelpers::Approximate<double>(exp, context));
  delete exp;

  return true;
}

bool RpnStack::doOperation(Poincare::StaticHierarchy<1> * exp, Poincare::Context &context) {
  if (size() < 1) {
    delete exp;
    return false;
  }

  Poincare::ListData listData;
  listData.pushExpression((*this)[size()-1].clone());
  exp->setArgument(&listData, 2, true);
  pop();

  push(PoincareHelpers::Approximate<double>(exp, context));
  delete exp;

  return true;
}

bool RpnStack::doOperation(Poincare::StaticHierarchy<2> * exp, Poincare::Context &context) {
  if (size() < 2) {
    delete exp;
    return false;
  }

  Poincare::ListData listData;
  listData.pushExpression((*this)[size()-2].clone());
  listData.pushExpression((*this)[size()-1].clone());
  exp->setArgument(&listData, 2, true);
  pop();
  pop();

  push(PoincareHelpers::Approximate<double>(exp, context));
  delete exp;

  return true;
}

}
