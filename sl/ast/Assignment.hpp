#ifndef SL_AST_ASSIGNMENT_HPP
#define SL_AST_ASSIGNMENT_HPP

#include <sl/ast/Expression.hpp>

namespace sl
{
namespace ast
{

class Variable;

class Assignment
{
public:

    Assignment(const Variable& var, const Expression& expr) : var_(&var), expr_(expr) { }

    const Variable& var() const { return *var_; }
    const Expression& expr() const { return expr_; }

private:

    const Variable *var_;
    Expression expr_;
};

}
}
#endif /* SL_AST_ASSIGNMENT_HPP */
