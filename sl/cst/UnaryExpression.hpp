#ifndef SL_CST_UNARYEXPRESSION_HPP
#define SL_CST_UNARYEXPRESSION_HPP

#include <boost/variant.hpp>
#include <sl/cst/Constant.hpp>
#include <sl/cst/Variable.hpp>

namespace sl
{
namespace cst
{

struct UnOpUnaryExpression;
struct FunctionCall;
struct Expression;

typedef boost::variant<
    Constant, 
    Variable, 
    boost::recursive_wrapper<FunctionCall>,
    boost::recursive_wrapper<Expression>,
    boost::recursive_wrapper<UnOpUnaryExpression>
> UnaryExpression;

}
}
#endif /* SL_CST_UNARYEXPRESSION_HPP */
