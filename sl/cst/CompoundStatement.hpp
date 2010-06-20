#ifndef SL_CST_COMPOUNDSTATEMENT_HPP
#define SL_CST_COMPOUNDSTATEMENT_HPP

#include <vector>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/cst/Statement.hpp>

namespace sl
{
namespace cst
{

struct CompoundStatement
{
    FilePosition openingBrace;
    std::vector<Statement> statements;
    FilePosition closingBrace;

    CompoundStatement() { }
    CompoundStatement(const std::vector<Statement>& s) : statements(s) { }
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::CompoundStatement,
    (sl::FilePosition, openingBrace)
    (std::vector<sl::cst::Statement>, statements)
    (sl::FilePosition, closingBrace)
)

#endif /* SL_CST_COMPOUNDSTATEMENT_HPP */
