#ifndef SL_CST_VARIABLEDELETE_HPP
#define SL_CST_VARIABLEDELETE_HPP

#include <string>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>

namespace sl
{
namespace cst
{

struct VariableDelete
{
    Identifier name;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::VariableDelete,
    (sl::cst::Identifier, name)
)

#endif /* SL_CST_VARIABLEDELETE_HPP */
