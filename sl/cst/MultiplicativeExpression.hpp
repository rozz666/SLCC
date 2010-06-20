#ifndef SL_CST_MULTIPLICATIVEEXPRESSION_HPP
#define SL_CST_MULTIPLICATIVEEXPRESSION_HPP

#include <vector>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/FilePosition.hpp>
#include <sl/cst/UnaryExpression.hpp>
#include <sl/cst/MulOpUnaryExpression.hpp>

namespace sl
{
namespace cst
{

struct MultiplicativeExpression
{
    FilePosition firstPos;
    UnaryExpression first;
    std::vector<MulOpUnaryExpression> next;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::MultiplicativeExpression,
    (sl::FilePosition, firstPos)
    (sl::cst::UnaryExpression, first)
    (std::vector<sl::cst::MulOpUnaryExpression>, next)
)
#endif /* SL_CST_MULTIPLICATIVEEXPRESSION_HPP */
