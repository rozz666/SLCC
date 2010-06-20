#include <sl/ast/BuiltinFunction.hpp>
#include <sl/ast/FunctionDef.hpp>
#include <sl/ast/functionName.hpp>

namespace sl
{
namespace ast
{

std::string functionName(const BuiltinFunction& f)
{
    return f.name();
}

std::string functionName(const FunctionDef& f)
{
    return f.name();
}

struct FunctionName : public boost::static_visitor<std::string>
{
    template <typename T>
    std::string operator()(const T& f) const { return ast::functionName(*f); }
};

std::string functionName(const FunctionRef& f)
{
    FunctionName fmn;
    return f.apply_visitor(fmn);
}


}
}
