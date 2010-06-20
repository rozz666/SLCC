#ifndef SL_CST_MULOPUNARYEXPRESSION_HPP
#define SL_CST_MULOPUNARYEXPRESSION_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/FilePosition.hpp>
#include <sl/cst/operators.hpp>
#include <sl/cst/UnaryExpression.hpp>

namespace sl
{
namespace cst
{

struct MulOpUnaryExpression
{
    FilePosition opPos;
    MulOp op;
    FilePosition exprPos;
    UnaryExpression expr;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::MulOpUnaryExpression,
    (sl::FilePosition, opPos)
    (sl::cst::MulOp, op)
    (sl::FilePosition, exprPos)
    (sl::cst::UnaryExpression, expr)
)

#endif /* SL_CST_MULOPUNARYEXPRESSION_HPP */
