#ifndef SL_AST_STATEMENT_HPP
#define SL_AST_STATEMENT_HPP

#include <boost/variant.hpp>
#include <sl/ast/Assignment.hpp>
#include <sl/ast/FunctionCall.hpp>
#include <sl/ast/ReturnStatement.hpp>
#include <sl/ast/VariableDecl.hpp>
#include <sl/ast/VariableDelete.hpp>

namespace sl
{
namespace ast
{

struct CompoundStatement;
class IfStatement;
class WhileLoop;

typedef boost::variant<
    boost::recursive_wrapper<CompoundStatement>, 
    Assignment, 
    FunctionCall, 
    ReturnStatement,
    boost::recursive_wrapper<IfStatement>,
    boost::recursive_wrapper<WhileLoop>,
    VariableDecl,
    VariableDelete
> Statement;

struct CompoundStatement
{
    std::vector<Statement> statements;
};

}
}

#include <sl/ast/IfStatement.hpp>
#include <sl/ast/WhileLoop.hpp>

#endif /* SL_AST_STATEMENT_HPP */
