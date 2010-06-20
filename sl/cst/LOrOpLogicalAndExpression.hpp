#ifndef SL_CST_LOROPLOGICALANDEXPRESSION_HPP
#define SL_CST_LOROPLOGICALANDEXPRESSION_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/FilePosition.hpp>
#include <sl/cst/operators.hpp>
#include <sl/cst/LogicalAndExpression.hpp>

namespace sl
{
namespace cst
{

struct LOrOpLogicalAndExpression
{
    FilePosition opPos;
    LOrOp op;
    FilePosition exprPos;
    LogicalAndExpression expr;
};


}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::LOrOpLogicalAndExpression,
    (sl::FilePosition, opPos)
    (sl::cst::LOrOp, op)
    (sl::FilePosition, exprPos)
    (sl::cst::LogicalAndExpression, expr)
)

#endif /* SL_CST_LOROPLOGICALANDEXPRESSION_HPP */
