#ifndef SL_CST_FUNCTIONRETURNTYPE_HPP
#define SL_CST_FUNCTIONRETURNTYPE_HPP

#include <boost/variant.hpp>
#include <sl/basicTypes.hpp>
#include <sl/cst/Expression.hpp>

namespace sl
{
namespace cst
{

typedef boost::variant<BasicType, Expression> FunctionReturnType;

}
}
#endif /* SL_CST_FUNCTIONRETURNTYPE_HPP */
