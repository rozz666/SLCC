#ifndef SL_AST_RETURNSTATEMENT_HPP
#define SL_AST_RETURNSTATEMENT_HPP

#include <sl/ast/Expression.hpp>

namespace sl
{
namespace ast
{

class ReturnStatement
{
public:

    ReturnStatement(const Expression& expr) : expr_(expr) { }

    const Expression& expr() const { return expr_; }

private:

    Expression expr_;
};

}
}
#endif /* SL_AST_RETURNSTATEMENT_HPP */
