#ifndef SL_CST_FUNCTIONCALL_HPP
#define SL_CST_FUNCTIONCALL_HPP

#include <vector>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/cst/Identifier.hpp>
#include <sl/cst/Expression.hpp>

namespace sl
{
namespace cst
{

struct FunctionCall
{
    Identifier name;
    std::vector<Expression> expr;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::FunctionCall,
    (sl::cst::Identifier, name)
    (std::vector<sl::cst::Expression>, expr)
)

#endif /* SL_CST_FUNCTIONCALL_HPP */
