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

    { ExpressionNode::Type::Store, 2, [](const Stack& s) -> Expression {
      auto symbol = s(0);
      if (symbol.type() == ExpressionNode::Type::Symbol) {
        return Store::Builder(s(1), *reinterpret_cast<Symbol*>(&symbol));
      }
      return Undefined::Builder();
    } },
  } ;

  for (size_t i = 0; i < sizeof(poincareOperators)/sizeof(poincareOperators[0]); i++) {
    if (poincareOperators[i].type == op) {
      return doOperation(std::move(poincareOperators[i].op(*this)), *context, poincareOperators[i].nargs);
    }
  }
  return I18n::Message::Warning;
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
  return I18n::Message::Warning;
}

struct MessageOperators {
  I18n::Message type;
  int nargs;
  Expression(*op)(const Stack&);
};

I18n::Message Stack::operator()(I18n::Message op, Context *context) {

  const static MessageOperators messageOperators[] = {
    { I18n::Message::AbsCommandWithArg, 1, [](const Stack& s) -> Expression { return AbsoluteValue::Builder(s(0)); } },
    { I18n::Message::AcoshCommandWithArg, 1, [](const Stack& s) -> Expression { return HyperbolicArcCosine::Builder(s(0)); } },
    { I18n::Message::ArgCommandWithArg, 1, [](const Stack& s) -> Expression { return ComplexArgument::Builder(s(0)); } },
    { I18n::Message::AsinhCommandWithArg, 1, [](const Stack& s) -> Expression { return HyperbolicArcSine::Builder(s(0)); } },
    { I18n::Message::AtanhCommandWithArg, 1, [](const Stack& s) -> Expression { return HyperbolicArcTangent::Builder(s(0)); } },
    { I18n::Message::BinomialCommandWithArg, 2, [](const Stack& s) -> Expression { return BinomialCoefficient::Builder(s(1), s(0)); } },
    { I18n::Message::BinomialCDFCommandWithArg, 3, [](const Stack& s) -> Expression { return BinomCDF::Builder(s(2), s(1), s(0)); } },
    { I18n::Message::BinomialPDFCommandWithArg, 3, [](const Stack& s) -> Expression { return BinomPDF::Builder(s(2), s(1), s(0)); } },
    { I18n::Message::CeilCommandWithArg, 1, [](const Stack& s) -> Expression { return Ceiling::Builder(s(0)); } },
    { I18n::Message::ConfidenceCommandWithArg, 2, [](const Stack& s) -> Expression { return ConfidenceInterval::Builder(s(1), s(0)); } },
    { I18n::Message::ConjCommandWithArg, 1, [](const Stack& s) -> Expression { return Conjugate::Builder(s(0)); } },
    { I18n::Message::CoshCommandWithArg, 1, [](const Stack& s) -> Expression { return HyperbolicCosine::Builder(s(0)); } },
    { I18n::Message::DeterminantCommandWithArg, 1, [](const Stack& s) -> Expression { return Determinant::Builder(s(0)); } },

    { I18n::Message::DiffCommandWithArg, 3, [](const Stack& s) -> Expression {
      auto symbol = s(1);
      if (symbol.type() == ExpressionNode::Type::Symbol) {
        return Derivative::Builder(s(2), *reinterpret_cast<Symbol*>(&symbol), s(0));
      }
      return Undefined::Builder();
    } },

    { I18n::Message::DimensionCommandWithArg, 1, [](const Stack& s) -> Expression { return MatrixDimension::Builder(s(0)); } },
    { I18n::Message::FactorCommandWithArg, 1, [](const Stack& s) -> Expression { return Factor::Builder(s(0)); } },
    { I18n::Message::FloorCommandWithArg, 1, [](const Stack& s) -> Expression { return Floor::Builder(s(0)); } },
    { I18n::Message::FracCommandWithArg, 1, [](const Stack& s) -> Expression { return FracPart::Builder(s(0)); } },
    { I18n::Message::GcdCommandWithArg, 2, [](const Stack& s) -> Expression { return GreatCommonDivisor::Builder(s(1), s(0)); } },
    { I18n::Message::ImCommandWithArg, 1, [](const Stack& s) -> Expression { return ImaginaryPart::Builder(s(0)); } },
    { I18n::Message::IndentityCommandWithArg, 1, [](const Stack& s) -> Expression { return MatrixIdentity::Builder(s(0)); } },

    { I18n::Message::IntCommandWithArg, 4, [](const Stack& s) -> Expression {
      auto symbol = s(2);
      if (symbol.type() == ExpressionNode::Type::Symbol) {
        return Integral::Builder(s(3), *reinterpret_cast<Symbol*>(&symbol), s(1), s(0));
      }
      return Undefined::Builder();
    } },

    { I18n::Message::InvBinomialCommandWithArg, 3, [](const Stack& s) -> Expression { return InvBinom::Builder(s(2), s(1), s(0)); } },
    { I18n::Message::InverseCommandWithArg, 1, [](const Stack& s) -> Expression { return MatrixInverse::Builder(s(0)); } },
    { I18n::Message::InvNormCommandWithArg, 3, [](const Stack& s) -> Expression { return InvNorm::Builder(s(2), s(1), s(0)); } },
    { I18n::Message::LcmCommandWithArg, 2, [](const Stack& s) -> Expression { return LeastCommonMultiple::Builder(s(1), s(0)); } },
    { I18n::Message::LogCommandWithArg, 2, [](const Stack& s) -> Expression { return Logarithm::Builder(s(1), s(0)); } },
    { I18n::Message::NormCDFCommandWithArg, 3, [](const Stack& s) -> Expression { return NormCDF::Builder(s(2), s(1), s(0)); } },
    { I18n::Message::NormCDF2CommandWithArg, 4, [](const Stack& s) -> Expression { return NormCDF2::Builder(s(3), s(2), s(1), s(0)); } },
    { I18n::Message::NormPDFCommandWithArg, 3, [](const Stack& s) -> Expression { return NormPDF::Builder(s(2), s(1), s(0)); } },
    { I18n::Message::Prediction95CommandWithArg, 2, [](const Stack& s) -> Expression { return SimplePredictionInterval::Builder(s(1), s(0)); } },
    { I18n::Message::PredictionCommandWithArg, 2, [](const Stack& s) -> Expression { return PredictionInterval::Builder(s(1), s(0)); } },

    { I18n::Message::ProductCommandWithArg, 4, [](const Stack& s) -> Expression {
      auto symbol = s(2);
      if (symbol.type() == ExpressionNode::Type::Symbol) {
        return Product::Builder(s(3), *reinterpret_cast<Symbol*>(&symbol), s(1), s(0));
      }
      return Undefined::Builder();
    } },

    { I18n::Message::QuoCommandWithArg, 2, [](const Stack& s) -> Expression { return DivisionQuotient::Builder(s(1), s(0)); } },
    { I18n::Message::RandintCommandWithArg, 2, [](const Stack& s) -> Expression { return Randint::Builder(s(1), s(0)); } },
    { I18n::Message::RandomCommandWithArg, 0, [](const Stack& s) -> Expression { return Random::Builder(); } },
    { I18n::Message::ReCommandWithArg, 1, [](const Stack& s) -> Expression { return RealPart::Builder(s(0)); } },
    { I18n::Message::RemCommandWithArg, 2, [](const Stack& s) -> Expression { return DivisionRemainder::Builder(s(1), s(0)); } },
    { I18n::Message::RootCommandWithArg, 2, [](const Stack& s) -> Expression { return NthRoot::Builder(s(1), s(0)); } },
    { I18n::Message::RoundCommandWithArg, 2, [](const Stack& s) -> Expression { return Round::Builder(s(1), s(0)); } },
    { I18n::Message::SinhCommandWithArg, 1, [](const Stack& s) -> Expression { return HyperbolicSine::Builder(s(0)); } },

    { I18n::Message::SumCommandWithArg, 4, [](const Stack& s) -> Expression {
      auto symbol = s(2);
      if (symbol.type() == ExpressionNode::Type::Symbol) {
        return Sum::Builder(s(3), *reinterpret_cast<Symbol*>(&symbol), s(1), s(0));
      }
      return Undefined::Builder();
    } },

    { I18n::Message::TanhCommandWithArg, 1, [](const Stack& s) -> Expression { return HyperbolicTangent::Builder(s(0)); } },
    { I18n::Message::TraceCommandWithArg, 1, [](const Stack& s) -> Expression { return MatrixTrace::Builder(s(0)); } },
    { I18n::Message::TransposeCommandWithArg, 1, [](const Stack& s) -> Expression { return MatrixTranspose::Builder(s(0)); } },
  } ;

  for (size_t i = 0; i < sizeof(messageOperators)/sizeof(messageOperators[0]); i++) {
    if (messageOperators[i].type == op) {
      return doOperation(std::move(messageOperators[i].op(*this)), *context, messageOperators[i].nargs);
    }
  }

  return I18n::Message::Warning;
}

void Stack::dropNth(size_t index) {
  if (!empty()) {
    for (size_t i = index; i < length(); i++) {
      m_stack[i] = m_stack[i+1];
    }
    m_length--;
  }
}

I18n::Message Stack::doOperation(Expression e, Context &context, int nargs) {
  if (e.isUndefined()) {
    return I18n::Message::SyntaxError;
  }
  else if (e.isRandom()) {
    // We don't want numbers on the stack to change on each evaluation.
    e = Shared::PoincareHelpers::Approximate<double>(e, &context);
  }
  if ((length() + nargs + 1) > (k_stackSize + nargs + 1)) {
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
  for (size_t i = k_stackSize-1; i > 0; i--) {
    m_stack[i] = m_stack[i-1];
  }
  m_stack[0] = e;
  m_length++;
  return I18n::Message::Default;
}

void Stack::pop() {
  for (size_t i = 0; i < k_stackSize-1; i++) {
    m_stack[i] = m_stack[i+1];
  }
  m_stack[k_stackSize-1] = Element();
  m_length -= m_length > 0 ? 1 : 0;
}

}
