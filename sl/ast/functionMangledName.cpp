#include <sl/ast/BuiltinFunction.hpp>
#include <sl/ast/FunctionDef.hpp>
#include <sl/ast/functionMangledName.hpp>

namespace sl
{
namespace ast
{

std::string functionMangledName(const BuiltinFunction& f)
{
    return functionMangledName(f.name(), f.suffix());
}

std::string functionMangledName(const FunctionDef& f)
{
    return functionMangledName(f.name(), f.suffix());
}

struct FunctionMangledName : public boost::static_visitor<std::string>
{
    template <typename T>
    std::string operator()(const T& f) const { return ast::functionMangledName(*f); }
};

std::string functionMangledName(const FunctionRef& f)
{
    FunctionMangledName fmn;
    return f.apply_visitor(fmn);
}

}
}
