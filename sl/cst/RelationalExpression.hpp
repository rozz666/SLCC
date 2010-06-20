#ifndef SL_CST_RELATIONALEXPRESSION_HPP
#define SL_CST_RELATIONALEXPRESSION_HPP

#include <vector>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/FilePosition.hpp>
#include <sl/cst/AdditiveExpression.hpp>
#include <sl/cst/RelOpAdditiveExpression.hpp>

namespace sl
{
namespace cst
{

struct RelationalExpression
{
    FilePosition firstPos;
    AdditiveExpression first;
    std::vector<RelOpAdditiveExpression> next;
};
}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::RelationalExpression,
    (sl::FilePosition, firstPos)
    (sl::cst::AdditiveExpression, first)
    (std::vector<sl::cst::RelOpAdditiveExpression>, next)
)

#endif /* SL_CST_RELATIONALEXPRESSION_HPP */
