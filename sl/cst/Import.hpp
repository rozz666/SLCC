#ifndef SL_CST_IMPORT_HPP
#define SL_CST_IMPORT_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/cst/Identifier.hpp>

namespace sl
{
namespace cst
{

struct Import
{
    Identifier module;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::Import,
    (sl::cst::Identifier, module)
)

#endif /* SL_CST_IMPORT_HPP */
