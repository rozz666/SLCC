#ifndef SL_CST_STATEMENT_HPP
#define SL_CST_STATEMENT_HPP

#include <boost/variant.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/cst/Assignment.hpp>
#include <sl/cst/FunctionCall.hpp>
#include <sl/cst/ReturnStatement.hpp>
#include <sl/cst/VariableDecl.hpp>
#include <sl/cst/VariableDelete.hpp>

namespace sl
{
namespace cst
{

struct CompoundStatement;
struct IfStatement;
struct WhileLoop;

typedef boost::variant<
    boost::recursive_wrapper<CompoundStatement>, 
    boost::recursive_wrapper<IfStatement>,
    boost::recursive_wrapper<WhileLoop>,
    Assignment, 
    FunctionCall, 
    ReturnStatement,
    VariableDecl,
    VariableDelete
> Statement;

}
}
#endif /* SL_CST_STATEMENT_HPP */
