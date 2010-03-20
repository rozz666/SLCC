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
    std::vector<Statement> statements;

    CompoundStatement() { }
    CompoundStatement(const std::vector<Statement>& s) : statements(s) { }
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::CompoundStatement,
    (std::vector<sl::cst::Statement>, statements)
)

#endif /* SL_CST_COMPOUNDSTATEMENT_HPP */
