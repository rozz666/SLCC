#ifndef SL_CST_LANDOPEQUALITYEXPRESSION_HPP
#define SL_CST_LANDOPEQUALITYEXPRESSION_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/FilePosition.hpp>
#include <sl/cst/operators.hpp>
#include <sl/cst/EqualityExpression.hpp>

namespace sl
{
namespace cst
{

struct LAndOpEqualityExpression
{
    FilePosition opPos;
    LAndOp op;
    FilePosition exprPos;
    EqualityExpression expr;
};

}
}
BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::LAndOpEqualityExpression,
    (sl::FilePosition, opPos)
    (sl::cst::LAndOp, op)
    (sl::FilePosition, exprPos)
    (sl::cst::EqualityExpression, expr)
)
#endif /* SL_CST_LANDOPEQUALITYEXPRESSION_HPP */
