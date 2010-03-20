#include <sl/ast/BuiltinFunction.hpp>
#include <sl/ast/FunctionDef.hpp>
#include <sl/ast/FunctionRef.hpp>

namespace sl
{
namespace ast
{

struct FunctionType : public boost::static_visitor<BasicType>
{
    template <typename T>
    BasicType operator()(const T& f) const { return f->type(); }
};

BasicType functionType(const FunctionRef& f)
{
    FunctionType ft;
    return f.apply_visitor(ft);
}

}
}
