#ifndef SL_CST_FUNCTIONPARAMETER_HPP
#define SL_CST_FUNCTIONPARAMETER_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/basicTypes.hpp>

namespace sl
{
namespace cst
{

struct FunctionParameter
{
    bool ref;
    BasicType type;
    Identifier name;
};
}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::FunctionParameter,
    (bool, ref)
    (sl::BasicType, type)
    (sl::cst::Identifier, name)
)
#endif /* SL_CST_FUNCTIONPARAMETER_HPP */
