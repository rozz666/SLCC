#ifndef SL_AST_WHILELOOP_HPP
#define SL_AST_WHILELOOP_HPP

#include <sl/ast/Expression.hpp>
#include <sl/ast/Statement.hpp>

namespace sl
{
namespace ast
{

class WhileLoop
{
public:

    WhileLoop(Expression&& cond, CompoundStatement&& body) : cond_(cond), body_(body) { }

    const Expression& cond() const { return cond_; }
    const CompoundStatement& body() const { return body_; }

private:

    Expression cond_;
    CompoundStatement body_;
};

}
}
#endif /* SL_AST_WHILELOOP_HPP */
