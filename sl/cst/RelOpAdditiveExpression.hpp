#ifndef SL_CST_RELOPADDITIVEEXPRESSION_HPP
#define SL_CST_RELOPADDITIVEEXPRESSION_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/FilePosition.hpp>
#include <sl/cst/operators.hpp>
#include <sl/cst/AdditiveExpression.hpp>

namespace sl
{
namespace cst
{

struct RelOpAdditiveExpression
{
    FilePosition opPos;
    RelOp op;
    FilePosition exprPos;
    AdditiveExpression expr;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::RelOpAdditiveExpression,
    (sl::FilePosition, opPos)
    (sl::cst::RelOp, op)
    (sl::FilePosition, exprPos)
    (sl::cst::AdditiveExpression, expr)
)

#endif /* SL_CST_RELOPADDITIVEEXPRESSION_HPP */
