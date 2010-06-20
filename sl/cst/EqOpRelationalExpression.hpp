#ifndef SL_CST_EQOPRELATIONALEXPRESSION_HPP
#define SL_CST_EQOPRELATIONALEXPRESSION_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/FilePosition.hpp>
#include <sl/cst/operators.hpp>
#include <sl/cst/RelationalExpression.hpp>

namespace sl
{
namespace cst
{

struct EqOpRelationalExpression
{
    FilePosition opPos;
    EqOp op;
    FilePosition exprPos;
    RelationalExpression expr;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::EqOpRelationalExpression,
    (sl::FilePosition, opPos)
    (sl::cst::EqOp, op)
    (sl::FilePosition, exprPos)
    (sl::cst::RelationalExpression, expr)
)

#endif /* SL_CST_EQOPRELATIONALEXPRESSION_HPP */
