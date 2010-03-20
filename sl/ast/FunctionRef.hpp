#ifndef SL_AST_FUNCTIONREF_HPP
#define SL_AST_FUNCTIONREF_HPP

#include <boost/variant.hpp>
#include <sl/basicTypes.hpp>

namespace sl
{
namespace ast
{

class BuiltinFunction;
class FunctionDef;

typedef boost::variant<
    const BuiltinFunction *,
    const FunctionDef *
> FunctionRef;

BasicType functionType(const FunctionRef& f);

}
}
#endif /* SL_AST_FUNCTIONREF_HPP */
