#ifndef SL_CST_FUNCTION_HPP
#define SL_CST_FUNCTION_HPP

#include <vector>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/cst/Identifier.hpp>
#include <sl/cst/FunctionParameter.hpp>
#include <sl/cst/FunctionReturnType.hpp>
#include <sl/cst/CompoundStatement.hpp>

namespace sl
{
namespace cst
{

struct Function
{
    Identifier name;
    std::vector<FunctionParameter> parameters;
    FunctionReturnType type;
    CompoundStatement body;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::Function,
    (sl::cst::Identifier, name)
    (std::vector<sl::cst::FunctionParameter>, parameters)
    (sl::cst::FunctionReturnType, type)
    (sl::cst::CompoundStatement, body)
)

#endif /* SL_CST_FUNCTION_HPP */
