#ifndef SL_AST_VARIABLEDELETE_HPP
#define SL_AST_VARIABLEDELETE_HPP

namespace sl
{
namespace ast
{

class Variable;

class VariableDelete
{
public:

    VariableDelete(const Variable& var) : var_(var) { }

    const Variable& var() const { return var_; }

private:

    Variable var_;
};

}
}
#endif /* SL_AST_VARIABLEDELETE_HPP */
