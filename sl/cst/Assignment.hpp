#ifndef SL_CST_ASSIGNMENT_HPP
#define SL_CST_ASSIGNMENT_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/cst/Identifier.hpp>
#include <sl/cst/Expression.hpp>

namespace sl
{
namespace cst
{

struct Assignment
{
    Identifier var;
    Expression expr;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::Assignment,
    (sl::cst::Identifier, var)
    (sl::cst::Expression, expr)
)

#endif /* SL_CST_ASSIGNMENT_HPP */
