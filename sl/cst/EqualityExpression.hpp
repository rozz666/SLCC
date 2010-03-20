#ifndef SL_CST_EQUALITYEXPRESSION_HPP
#define SL_CST_EQUALITYEXPRESSION_HPP

#include <vector>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/FilePosition.hpp>
#include <sl/cst/RelationalExpression.hpp>
#include <sl/cst/EqOpRelationalExpression.hpp>

namespace sl
{
namespace cst
{

struct EqualityExpression
{
    FilePosition firstPos;
    RelationalExpression first;
    std::vector<EqOpRelationalExpression> next;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::EqualityExpression,
    (sl::FilePosition, firstPos)
    (sl::cst::RelationalExpression, first)
    (std::vector<sl::cst::EqOpRelationalExpression>, next)
)
#endif /* SL_CST_EQUALITYEXPRESSION_HPP */
