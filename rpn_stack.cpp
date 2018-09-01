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

bool RpnStack::push(const char *text) {
  Poincare::Expression * exp = Poincare::Expression::parse(text);
  if (exp == nullptr) {
    return false;
  }
  *m_curStackPtr++ = exp;
  return true;
}

void RpnStack::push(Poincare::Expression * exp) {
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
