#ifndef SL_AST_BUILTINFUNCTION_HPP
#define SL_AST_BUILTINFUNCTION_HPP

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sl/basicTypes.hpp>
#include <sl/ast/functionSuffix.hpp>

namespace sl
{
namespace ast
{

class BuiltinFunction
{
public:

    BuiltinFunction(const std::string& name, BasicType type) : name_(name), type_(type) { }

    BuiltinFunction(const std::string& name, BasicType arg0, BasicType type)
        : name_(name), type_(type)
    {
        argTypes_.resize(1, arg0);
        suffix_ = functionSuffix(argTypes_);
    }

    BuiltinFunction(const std::string& name, BasicType arg0, BasicType arg1, BasicType type) : name_(name), type_(type)
    {
        argTypes_.reserve(2);
        argTypes_.push_back(arg0);
        argTypes_.push_back(arg1);
        suffix_ = functionSuffix(argTypes_);
    }

    const std::string& name() const { return name_; }
    const std::string& suffix() const { return suffix_; }
    BasicType type() const { return type_; }
    std::vector<sl::BasicType> argTypes() const { return argTypes_; }

private:

    std::string name_;
    std::string suffix_;     
    BasicType type_;
    std::vector<sl::BasicType> argTypes_;
};

}
}
#endif /* SL_AST_BUILTINFUNCTION_HPP */
