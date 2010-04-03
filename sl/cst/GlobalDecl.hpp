#ifndef SL_CST_GLOBALDECL_HPP
#define SL_CST_GLOBALDECL_HPP

#include <vector>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/cst/Function.hpp>
#include <sl/cst/Import.hpp>

namespace sl
{
namespace cst
{                

typedef boost::variant<Function, Import> GlobalDecl;

}
}

#endif /* SL_CST_MODULE_HPP */
