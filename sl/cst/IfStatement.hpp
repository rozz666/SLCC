#ifndef SL_CST_IFSTATEMENT_HPP
#define SL_CST_IFSTATEMENT_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/cst/Expression.hpp>
#include <sl/cst/CompoundStatement.hpp>

namespace sl
{
namespace cst
{

struct IfStatement
{
    Expression cond;
    CompoundStatement onTrue;
    boost::optional<CompoundStatement> onFalse;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::IfStatement,
    (sl::cst::Expression, cond)
    (sl::cst::CompoundStatement, onTrue)
    (boost::optional<sl::cst::CompoundStatement>, onFalse)
)

#endif /* SL_CST_IFSTATEMENT_HPP */
