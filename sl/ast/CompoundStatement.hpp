// UGLY HACK
#include <sl/ast/Statement.hpp>

#ifndef SL_AST_COMPOUNDSTATEMENT_HPP
#define SL_AST_COMPOUNDSTATEMENT_HPP

#include <vector>

namespace sl
{
namespace ast
{

struct CompoundStatement
{
    std::vector<Statement> statements;
};

}
}
#endif /* SL_AST_COMPOUNDSTATEMENT_HPP */
