#ifndef SL_CST_VARIABLEDECL_HPP
#define SL_CST_VARIABLEDECL_HPP

#include <boost/optional.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/basicTypes.hpp>
#include <sl/cst/Identifier.hpp>
#include <sl/cst/Expression.hpp>


namespace sl
{
namespace cst
{

struct VariableDecl
{
    boost::optional<BasicType> type;
    Identifier name;
    boost::optional<Expression> expr;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::VariableDecl,
    (boost::optional<sl::BasicType>, type)
    (sl::cst::Identifier, name)
    (boost::optional<sl::cst::Expression>, expr)
)

#endif /* SL_CST_VARIABLEDECL_HPP */
