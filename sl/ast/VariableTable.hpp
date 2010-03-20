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
    typedef std::unordered_map<std::string, const Variable *> C; 

public:

    bool insert(const Variable& var)
    {
        return vars_.insert(std::make_pair(var.name(), &var)).second;
    }

    const Variable *find(const std::string& name) const
    {
        C::const_iterator it = vars_.find(name);

        return (it != vars_.end()) ? it->second : nullptr;
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
