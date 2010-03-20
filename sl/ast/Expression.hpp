#ifndef SL_AST_EXPRESSION_HPP
#define SL_AST_EXPRESSION_HPP

#include <boost/variant.hpp>
#include <sl/ast/Constant.hpp>
#include <sl/ast/Variable.hpp>

namespace sl
{
namespace ast
{

class FunctionCall;
class Cast;

typedef boost::variant<
    Constant,
    const Variable *,
    boost::recursive_wrapper<FunctionCall>,
    boost::recursive_wrapper<Cast>
> Expression;

}
}

#include <sl/ast/FunctionCall.hpp>
#include <sl/ast/Cast.hpp>

#endif /* SL_AST_EXPRESSION_HPP */
