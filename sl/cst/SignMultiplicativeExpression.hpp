#ifndef SL_CST_SIGNMULTIPLICATIVEEXPRESSION_HPP
#define SL_CST_SIGNMULTIPLICATIVEEXPRESSION_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/FilePosition.hpp>
#include <sl/cst/operators.hpp>
#include <sl/cst/MultiplicativeExpression.hpp>

namespace sl
{
namespace cst
{

struct SignMultiplicativeExpression
{
    FilePosition opPos;
    UnOp op;
    FilePosition exprPos;
    MultiplicativeExpression expr;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::SignMultiplicativeExpression,
    (sl::FilePosition, opPos)
    (sl::cst::UnOp, op)
    (sl::FilePosition, exprPos)
    (sl::cst::MultiplicativeExpression, expr)
)

#endif /* SL_CST_SIGNMULTIPLICATIVEEXPRESSION_HPP */
