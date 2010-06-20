#ifndef SL_AST_FUNCTIONDEF_HPP
#define SL_AST_FUNCTIONDEF_HPP

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>
#include <sl/basicTypes.hpp>
#include <sl/ast/functionSuffix.hpp>
#include <sl/ast/Statement.hpp>

namespace sl
{
namespace ast
{

class FunctionDef
{
public:

    typedef std::vector<std::shared_ptr<Variable> > ParameterContainer;
   
    FunctionDef(const std::string& name, const FilePosition& pos, ParameterContainer&& parameters) 
        : name_(name), suffix_(functionSuffix(parameters)), pos_(pos)
    {
        parameters.swap(parameters_);
    }

    const std::string& name() const { return name_; }
    const std::string& suffix() const { return suffix_; }
    FilePosition pos() const { return pos_; }
    BasicType type() const { return *type_; }
    void type(BasicType val) { type_ = val; }

    const ParameterContainer& parameters() const { return parameters_; }

    const boost::optional<CompoundStatement>& body() const { return body_; }

    void body(CompoundStatement&& c)
    {
        body_ = c;
    }

private:

    std::string name_;
    std::string suffix_;
    FilePosition pos_;
    boost::optional<BasicType> type_;
    ParameterContainer parameters_;
    boost::optional<CompoundStatement> body_;
};

}
}
#endif /* SL_AST_FUNCTIONDEF_HPP */
