#ifndef SL_AST_VARIABLETABLE_HPP
#define SL_AST_VARIABLETABLE_HPP

#include <string>
#include <unordered_map>
#include <sl/ast/Variable.hpp>

namespace sl
{
namespace ast
{

class VariableTable
{
    typedef std::unordered_map<std::string, Variable> C; 

public:

    bool insert(const Variable& var)
    {
        return vars_.insert(std::make_pair(var.name(), var)).second;
    }

    boost::optional<Variable> find(const std::string& name) const
    {
        C::const_iterator it = vars_.find(name);

        if (it == vars_.end()) return boost::none;
        
        return it->second;
    }

    void erase(const std::string& name)
    {
        vars_.erase(name);
    }

private:

    C vars_;
};

}
}
#endif /* SL_AST_VARIABLETABLE_HPP */
