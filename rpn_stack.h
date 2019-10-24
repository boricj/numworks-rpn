#ifndef RPN_STACK_H
#define RPN_STACK_H

#include <escher.h>
#include <poincare/expression.h>
#include <apps/i18n.h>
#include <stddef.h>

namespace Rpn {

class Stack {
public:
  constexpr static size_t k_stackSize = 8;
  constexpr static int k_expressionSize = TextField::maxBufferSize();

  enum StackOperation {
    DUP,
    SWAP,
    ROT,
    OVER,
    POP,
    CLEAR
  } ;

  enum SpecialOperation {
    Exp,
    CommonLogarithm,
    Square
  } ;

private:
  struct Element {
    Element();
    Element(Poincare::Expression &exp, Poincare::Context &context);

    char expression[k_expressionSize];
    char approximate[k_expressionSize];
    KDCoordinate expressionHeight;
    KDCoordinate approximateHeight;
  };

public:
  Stack() = default;

  const char* operator[](size_t idx) const { return approximate ? m_stack[idx].approximate : m_stack[idx].expression; }
  Poincare::Expression operator()(size_t idx) const { return Poincare::Expression::Parse(m_stack[idx].expression); }
  KDCoordinate height(size_t idx) const { return approximate ? m_stack[idx].approximateHeight : m_stack[idx].expressionHeight; }

  I18n::Message operator()(const char* text, Poincare::Context *context);
  I18n::Message operator()(StackOperation op);
  I18n::Message operator()(SpecialOperation op, Poincare::Context *context);
  I18n::Message operator()(Poincare::ExpressionNode::Type op, Poincare::Context *context);
  I18n::Message operator()(I18n::Message op, Poincare::Context *context);

  void dropNth(size_t index);

  size_t length() const { return m_length; }
  bool full() const { return m_length == k_stackSize; }
  bool empty() const { return m_length == 0; }

  bool approximate;

private:
  I18n::Message doOperation(Poincare::Expression e, Poincare::Context &context, int nargs);
  I18n::Message push(Poincare::Expression e, Poincare::Context &context);
  I18n::Message push(Element exp);
  void pop();

  Element m_stack[k_stackSize];
  size_t m_length;
};

}

#endif
