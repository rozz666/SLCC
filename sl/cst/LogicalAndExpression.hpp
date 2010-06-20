#ifndef SL_CST_LOGICALANDEXPRESSION_HPP
#define SL_CST_LOGICALANDEXPRESSION_HPP

#include <vector>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/FilePosition.hpp>
#include <sl/cst/EqualityExpression.hpp>
#include <sl/cst/LAndOpEqualityExpression.hpp>

namespace sl
{
namespace cst
{

struct LogicalAndExpression
{
    FilePosition firstPos;
    EqualityExpression first;
    std::vector<LAndOpEqualityExpression> next;
};

}
}
BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::LogicalAndExpression,
    (sl::FilePosition, firstPos)
    (sl::cst::EqualityExpression, first)
    (std::vector<sl::cst::LAndOpEqualityExpression>, next)
)
#endif /* SL_CST_LOGICALANDEXPRESSION_HPP */
