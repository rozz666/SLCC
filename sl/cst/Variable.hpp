#ifndef SL_CST_VARIABLE_HPP
#define SL_CST_VARIABLE_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/cst/Identifier.hpp>

namespace sl
{
namespace cst
{

struct Variable
{
    Identifier name;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::Variable,
    (sl::cst::Identifier, name)
)

#endif /* SL_CST_VARIABLE_HPP */
