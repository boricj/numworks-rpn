#include "rpn_stack.h"
#include <string.h>

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

void RpnStack::pop() {
  assert(!empty());
  --m_curStackPtr;
  delete *m_curStackPtr;
  *m_curStackPtr = nullptr;
}

}
