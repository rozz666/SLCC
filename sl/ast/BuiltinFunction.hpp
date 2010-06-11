#ifndef SL_AST_BUILTINFUNCTION_HPP
#define SL_AST_BUILTINFUNCTION_HPP

#include <string>
#include <vector>
#include <sl/basicTypes.hpp>
#include <sl/ast/types.hpp>

namespace sl
{
namespace ast
{

class BuiltinFunction
{
public:

    BuiltinFunction(const std::string& name, BasicType type) : name_(name), type_(type) { }

    BuiltinFunction(const std::string& name, BasicType arg0, BasicType type)
        : name_(name), suffix_(typeSuffix(arg0)), type_(type)
    {
        argTypes_.resize(1, arg0);
    }

    BuiltinFunction(const std::string& name, BasicType arg0, BasicType arg1, BasicType type) : name_(name), type_(type)
    {
        suffix_ = typeSuffix(arg0);
        suffix_ += typeSuffix(arg1);
        argTypes_.reserve(2);
        argTypes_.push_back(arg0);
        argTypes_.push_back(arg1);
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
