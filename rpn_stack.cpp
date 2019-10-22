#include "rpn_stack.h"
#include "../shared/poincare_helpers.h"
#include <string.h>
#include <utility>
#include <poincare_nodes.h>
#include <poincare/preferences.h>

using namespace Poincare;

namespace Rpn {

Stack::Element::Element() : expression("0"), approximate("0"), expressionHeight(KDFont::LargeFont->glyphSize().height()), approximateHeight(KDFont::LargeFont->glyphSize().height()) {
}

Stack::Element::Element(Expression &exp, Context &context) {
  Expression exact = exp.simplify(&context, Preferences::sharedPreferences()->complexFormat(), Preferences::sharedPreferences()->angleUnit());
  exact.serialize(expression, k_expressionSize);
  expressionHeight = exact.createLayout(
    Preferences::sharedPreferences()->displayMode(),
    Preferences::sharedPreferences()->numberOfSignificantDigits()
  ).layoutSize().height();

  Expression approx = exact.approximate<double>(
    &context,
    Preferences::sharedPreferences()->complexFormat(),
    Preferences::sharedPreferences()->angleUnit()
  );
  approx.serialize(approximate, k_expressionSize);
  approximateHeight = approx.createLayout(
    Preferences::sharedPreferences()->displayMode(),
    Preferences::sharedPreferences()->numberOfSignificantDigits()
  ).layoutSize().height();
}

I18n::Message Stack::operator()(const char *text, Context *context) {
  Expression exp = Expression::Parse(text);
  if (exp.isUninitialized()) {
    return I18n::Message::SyntaxError;
  }
  return push(exp, *context);
}

I18n::Message Stack::operator()(StackOperation op) {
  Element a, b, c;
  I18n::Message r = I18n::Message::Default;

  switch (op) {
    case DUP:
      a = m_stack[0];
      r = push(a);
      break;
    case SWAP:
      a = m_stack[0];
      b = m_stack[1];
      pop();
      pop();
      push(a);
      push(b);
      break;
    case ROT:
      a = m_stack[0];
      b = m_stack[1];
      c = m_stack[2];
      pop();
      pop();
      pop();
      push(b);
      push(a);
      push(c);
      break;
    case OVER:
      a = m_stack[1];
      r = push(a);
      break;
    case POP:
      pop();
      break;
    case CLEAR:
      while (!empty()) {
        pop();
      }
      break;
  }
  return r;
}

struct PoincareOperators {
  ExpressionNode::Type type;
  int nargs;
  Expression(*op)(const Stack&);
};

I18n::Message Stack::operator()(ExpressionNode::Type op, Context *context) {
  const static PoincareOperators poincareOperators[] = {
    { ExpressionNode::Type::Addition, 2, [](const Stack& s) -> Expression { return Addition::Builder(s(1), s(0)); } },
    { ExpressionNode::Type::Subtraction, 2, [](const Stack& s) -> Expression { return Subtraction::Builder(s(1), s(0)); } },
    { ExpressionNode::Type::Multiplication, 2, [](const Stack& s) -> Expression { return Multiplication::Builder(s(1), s(0)); } },
    { ExpressionNode::Type::Division, 2, [](const Stack& s) -> Expression { return Division::Builder(s(1), s(0)); } },

    { ExpressionNode::Type::NaperianLogarithm, 1, [](const Stack& s) -> Expression { return NaperianLogarithm::Builder(s(0)); } },
    { ExpressionNode::Type::Logarithm, 2, [](const Stack& s) -> Expression { return Logarithm::Builder(s(1), s(0)); } },
    { ExpressionNode::Type::Power, 2, [](const Stack& s) -> Expression { return Power::Builder(s(1), s(0)); } },
    { ExpressionNode::Type::Sine, 1, [](const Stack& s) -> Expression { return Sine::Builder(s(0)); } },
    { ExpressionNode::Type::Cosine, 1, [](const Stack& s) -> Expression { return Cosine::Builder(s(0)); } },
    { ExpressionNode::Type::Tangent, 1, [](const Stack& s) -> Expression { return Tangent::Builder(s(0)); } },
    { ExpressionNode::Type::ArcSine, 1, [](const Stack& s) -> Expression { return ArcSine::Builder(s(0)); } },
    { ExpressionNode::Type::ArcCosine, 1, [](const Stack& s) -> Expression { return ArcCosine::Builder(s(0)); } },
    { ExpressionNode::Type::ArcTangent, 1, [](const Stack& s) -> Expression { return ArcTangent::Builder(s(0)); } },
    { ExpressionNode::Type::SquareRoot, 1, [](const Stack& s) -> Expression { return SquareRoot::Builder(s(0)); } },
    { ExpressionNode::Type::Opposite, 1, [](const Stack& s) -> Expression { return Opposite::Builder(s(0)); } },
  } ;

  for (size_t i = 0; i < sizeof(poincareOperators)/sizeof(poincareOperators[0]); i++) {
    if (poincareOperators[i].type == op) {
      return doOperation(std::move(poincareOperators[i].op(*this)), *context, poincareOperators[i].nargs);
    }
  }
  return I18n::Message::Default;
}

struct SpecialOperators {
  Stack::SpecialOperation type;
  int nargs;
  Expression(*op)(const Stack&);
};

I18n::Message Stack::operator()(Stack::SpecialOperation op, Context *context) {
  const static SpecialOperators specialOperators[] = {
    { Stack::Exp, 1, [](const Stack& s) -> Expression { return Power::Builder(Constant::Builder(UCodePointScriptSmallE), s(0)); } },
    { Stack::CommonLogarithm, 1, [](const Stack& s) -> Expression { return CommonLogarithm::Builder(s(0)); } },
    { Stack::Square, 1, [](const Stack& s) -> Expression { return Power::Builder(s(0), Rational::Builder(2)); } },
  } ;

  for (size_t i = 0; i < sizeof(specialOperators)/sizeof(specialOperators[0]); i++) {
    if (specialOperators[i].type == op) {
      return doOperation(std::move(specialOperators[i].op(*this)), *context, specialOperators[i].nargs);
    }
  }
  return I18n::Message::Default;
}

I18n::Message Stack::doOperation(Expression e, Context &context, int nargs) {
  if ((length() - nargs - 1) >= (k_stackSize - 1)) {
    return I18n::Message::StorageMemoryFull1;
  }
  while(nargs) {
    pop();
    nargs--;
  }
  return push(std::move(e), context);
}

I18n::Message Stack::push(Expression e, Context &context) {
  Element a(e, context);
  return push(std::move(a));
}

I18n::Message Stack::push(Element e) {
  if (full()) {
    return I18n::Message::StorageMemoryFull1;
  }
  for (int i = k_stackSize-1; i > 0; i--) {
    m_stack[i] = m_stack[i-1];
  }
  m_stack[0] = e;
  m_length++;
  return I18n::Message::Default;
}

void Stack::pop() {
  for (int i = 0; i < k_stackSize-1; i++) {
    m_stack[i] = m_stack[i+1];
  }
  m_stack[k_stackSize-1] = Element();
  m_length -= m_length > 0 ? 1 : 0;
}

}
