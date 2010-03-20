#ifndef SL_CST_RETURNSTATEMENT_HPP
#define SL_CST_RETURNSTATEMENT_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/cst/Expression.hpp>

namespace sl
{
namespace cst
{

struct ReturnStatement
{
    Expression expr;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::ReturnStatement,
    (sl::cst::Expression, expr)
)

#endif /* SL_CST_RETURNSTATEMENT_HPP */
