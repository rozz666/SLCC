#ifndef SL_CST_MODULE_HPP
#define SL_CST_MODULE_HPP

#include <vector>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/cst/Identifier.hpp>
#include <sl/cst/GlobalDecl.hpp>

namespace sl
{
namespace cst
{                

struct Module
{
    Identifier name;
    std::vector<GlobalDecl> decls;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::Module,
    (sl::cst::Identifier, name)
    (std::vector<sl::cst::GlobalDecl>, decls)
)

#endif /* SL_CST_MODULE_HPP */
