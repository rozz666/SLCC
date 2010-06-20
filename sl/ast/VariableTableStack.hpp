#ifndef SL_AST_VARIABLETABLESTACK_HPP
#define SL_AST_VARIABLETABLESTACK_HPP

#include <vector>
#include <sl/ast/VariableTable.hpp>

namespace sl
{
namespace ast
{

class VariableTableStack
{
    typedef std::vector<VariableTable> C;

public:

    VariableTableStack()
    {
        push();
    }

    bool insert(const Variable& var)
    {
        return stack_.back().insert(var);
    }

    boost::optional<Variable> find(const std::string& name) const
    {
        for (C::const_reverse_iterator it = stack_.rbegin(); it != stack_.rend(); ++it)
        {
            if (boost::optional<Variable> v = it->find(name)) return v;
        }

        return boost::none;
    }

    boost::optional<Variable> findInScope(const std::string& name) const
    {
        return stack_.back().find(name);
    }

    void eraseInScope(const std::string& name)
    {
        stack_.back().erase(name);
    }

    void push()
    {
        stack_.push_back(VariableTable());
    }

    void pop()
    {
        stack_.pop_back();
    }

private:

    C stack_;
};

}
}
#endif /* SL_AST_VARIABLETABLESTACK_HPP */
