#ifndef SL_AST_FUNCTIONARGTYPES_HPP
#define SL_AST_FUNCTIONARGTYPES_HPP

#include <vector>
#include <sl/basicTypes.hpp>
#include <sl/ast/FunctionRef.hpp>

namespace sl
{
namespace ast
{

std::vector<sl::BasicType> functionArgTypes(const BuiltinFunction& f);
std::vector<sl::BasicType> functionArgTypes(const FunctionDef& f);
std::vector<sl::BasicType> functionArgTypes(const FunctionRef& f);

}
}

#endif /* SL_AST_FUNCTIONARGTYPES_HPP */