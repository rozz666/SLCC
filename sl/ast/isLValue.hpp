#ifndef SL_AST_ISLVALUE_HPP
#define SL_AST_ISLVALUE_HPP

#include <sl/ast/Expression.hpp>

namespace sl
{
namespace ast
{

bool isLValue(const Expression& e);

}
}
#endif /* SL_AST_ISLVALUE_HPP */
