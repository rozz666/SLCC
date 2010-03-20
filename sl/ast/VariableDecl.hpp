#ifndef SL_AST_VARIABLEDECL_HPP
#define SL_AST_VARIABLEDECL_HPP

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <sl/FilePosition.hpp>
#include <sl/basicTypes.hpp>
#include <sl/ast/Expression.hpp>
#include <sl/ast/Variable.hpp>

namespace sl
{
namespace ast
{

class VariableDecl  
{
public:

    VariableDecl(const std::string& name, const FilePosition& pos, BasicType type) : var_(new Variable(name, pos, type, false)) { }
    VariableDecl(const std::string& name, const FilePosition& pos, BasicType type, const Expression& expr) : var_(new Variable(name, pos, type, false)), expr_(expr) { }

    Variable& var() { return *var_; }
    const Variable& var() const { return *var_; }

    const boost::optional<Expression>& expr() const { return expr_; }

private:

    std::shared_ptr<Variable> var_;
    boost::optional<Expression> expr_;
};

}
}
#endif /* SL_AST_VARIABLEDECL_HPP */
