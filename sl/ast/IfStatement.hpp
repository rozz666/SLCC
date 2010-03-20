#ifndef SL_AST_IFSTATEMENT_HPP
#define SL_AST_IFSTATEMENT_HPP

#include <boost/optional.hpp>
#include <sl/ast/Expression.hpp>
#include <sl/ast/CompoundStatement.hpp>

namespace sl
{
namespace ast
{

class IfStatement
{
public:

    IfStatement(Expression&& cond, CompoundStatement&& onTrue) : cond_(cond), onTrue_(onTrue) { }
    IfStatement(Expression&& cond, CompoundStatement&& onTrue, CompoundStatement&& onFalse) : cond_(cond), onTrue_(onTrue), onFalse_(onFalse) { }

    const Expression& cond() const { return cond_; }
    const CompoundStatement& onTrue() const { return onTrue_; }
    const boost::optional<CompoundStatement>& onFalse() const { return onFalse_; }

private:

    Expression cond_;
    CompoundStatement onTrue_;
    boost::optional<CompoundStatement> onFalse_;
};

}
}
#endif /* SL_AST_IFSTATEMENT_HPP */
