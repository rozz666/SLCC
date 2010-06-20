#ifndef SL_AST_MODULE_HPP
#define SL_AST_MODULE_HPP

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <sl/ast/FunctionDef.hpp>

namespace sl
{               
namespace ast
{

class Module : boost::noncopyable
{
public:

    typedef boost::ptr_vector<FunctionDef> FunctionContainer;

    Module(Module&& right)
    {
        name_.swap(right.name_);
        functions_.swap(right.functions_);
    }

    Module(const std::string& name) : name_(name) { }

    void insertFunction(std::auto_ptr<FunctionDef> function)
    {
        functions_.push_back(function);
    }

    const std::string name() const { return name_; }

    const FunctionContainer& functions() const { return functions_; }

private:

    std::string name_;
    FunctionContainer functions_;
};

}
}
#endif /* SL_AST_MODULE_HPP */
