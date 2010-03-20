#ifndef SL_AST_FUNCTIONTABLE_HPP
#define SL_AST_FUNCTIONTABLE_HPP

#include <string>
#include <unordered_map>
#include <sl/ast/FunctionRef.hpp>
#include <sl/ast/functionMangledName.hpp>

namespace sl
{
namespace ast
{

class FunctionTable
{
    typedef std::unordered_map<std::string, FunctionRef> C;

public:

    FunctionTable() { }

    bool insert(const FunctionRef& f)
    {
        return functions_.insert(std::make_pair(functionMangledName(f), f)).second;
    }

    boost::optional<FunctionRef> find(const std::string& name) const
    {
        C::const_iterator it = functions_.find(name);

        if (it != functions_.end()) return it->second;
        else return boost::none;
    }

private:

    C functions_;
};

}
}
#endif /* SL_AST_FUNCTIONTABLE_HPP */
