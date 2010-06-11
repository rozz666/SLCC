#ifndef SL_AST_FUNCTIONNAME_HPP
#define SL_AST_FUNCTIONNAME_HPP

#include <string>
#include <sl/ast/FunctionRef.hpp>

namespace sl
{
namespace ast
{

std::string functionName(const BuiltinFunction& f);
std::string functionName(const FunctionDef& f);
std::string functionName(const FunctionRef& f);

}
}

#endif /* SL_AST_FUNCTIONNAME_HPP */
