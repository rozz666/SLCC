#ifndef SL_CST_ADDITIVEEXPRESSION_HPP
#define SL_CST_ADDITIVEEXPRESSION_HPP

#include <vector>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/FilePosition.hpp>
#include <sl/cst/MultiplicativeExpression.hpp>
#include <sl/cst/SignMultiplicativeExpression.hpp>

namespace sl
{
namespace cst
{

struct AdditiveExpression
{
    FilePosition firstPos;
    MultiplicativeExpression first;
    std::vector<SignMultiplicativeExpression> next;
};
}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::AdditiveExpression,
    (sl::FilePosition, firstPos)
    (sl::cst::MultiplicativeExpression, first)
    (std::vector<sl::cst::SignMultiplicativeExpression>, next)
)
#endif /* SL_CST_ADDITIVEEXPRESSION_HPP */
