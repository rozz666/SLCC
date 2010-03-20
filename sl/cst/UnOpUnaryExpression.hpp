#ifndef SL_CST_UNOPUNARYEXPRESSION_HPP
#define SL_CST_UNOPUNARYEXPRESSION_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/FilePosition.hpp>
#include <sl/cst/operators.hpp>
#include <sl/cst/UnaryExpression.hpp>

namespace sl
{
namespace cst
{

struct UnOpUnaryExpression
{
    FilePosition opPos;
    UnOp op;
    FilePosition exprPos;
    UnaryExpression expr;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::UnOpUnaryExpression,
    (sl::FilePosition, opPos)
    (sl::cst::UnOp, op)
    (sl::FilePosition, exprPos)
    (sl::cst::UnaryExpression, expr)
)

#endif /* SL_CST_UNOPUNARYEXPRESSION_HPP */
