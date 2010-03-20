#ifndef SL_CST_WHILELOOP_HPP
#define SL_CST_WHILELOOP_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/cst/Expression.hpp>
#include <sl/cst/CompoundStatement.hpp>

namespace sl
{
namespace cst
{

struct WhileLoop
{
    Expression cond;
    CompoundStatement body;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::WhileLoop,
    (sl::cst::Expression, cond)
    (sl::cst::CompoundStatement, body)
)

#endif /* SL_CST_WHILELOOP_HPP */
