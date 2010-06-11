#include <boost/range/algorithm/transform.hpp>
#include <boost/bind.hpp>
#include <sl/ast/BuiltinFunction.hpp>
#include <sl/ast/FunctionDef.hpp>
#include <sl/ast/functionArgTypes.hpp>

namespace sl
{
namespace ast
{

std::vector<sl::BasicType> functionArgTypes(const BuiltinFunction& f)
{
    return f.argTypes();
}

std::vector<sl::BasicType> functionArgTypes(const FunctionDef& f)
{
    std::vector<sl::BasicType> argTypes;
    argTypes.reserve(f.parameters().size());

    boost::transform(f.parameters(), std::back_inserter(argTypes), std::mem_fn(&Variable::type));

    return argTypes;
}

struct FunctionArgTypes : public boost::static_visitor<std::vector<sl::BasicType> >
{
    template <typename T>
    std::vector<sl::BasicType> operator()(const T& f) const { return ast::functionArgTypes(*f); }
};

std::vector<sl::BasicType> functionArgTypes(const FunctionRef& f)
{
    FunctionArgTypes fat;
    return f.apply_visitor(fat);
}

}
}
