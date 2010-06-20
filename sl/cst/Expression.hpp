#ifndef SL_CST_EXPRESSION_HPP
#define SL_CST_EXPRESSION_HPP

#include <vector>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/FilePosition.hpp>
#include <sl/cst/LogicalAndExpression.hpp>
#include <sl/cst/LOrOpLogicalAndExpression.hpp>

namespace sl
{
namespace cst
{

struct Expression
{
    FilePosition firstPos;
    LogicalAndExpression first;
    std::vector<LOrOpLogicalAndExpression> next;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::Expression,
    (sl::FilePosition, firstPos)
    (sl::cst::LogicalAndExpression, first)
    (std::vector<sl::cst::LOrOpLogicalAndExpression>, next)
)

#endif /* SL_CST_EXPRESSION_HPP */
