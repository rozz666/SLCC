#ifndef SL_AST_FUNCTIONMANGLEDNAME_HPP
#define SL_AST_FUNCTIONMANGLEDNAME_HPP

#include <string>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/is_same.hpp>
#include <sl/ast/functionSuffix.hpp>
#include <sl/ast/Variable.hpp>
#include <sl/ast/FunctionRef.hpp>

namespace sl
{
namespace ast
{

inline std::string functionMangledName(const std::string& name, const std::string& suffix)
{
    return name + "$" + suffix;
}

template <typename Container>
inline
typename boost::enable_if<
    boost::is_same<typename boost::remove_reference<typename Container::reference>::type, Variable>,
    std::string
>::type
functionMangledName(const std::string& name, const Container& params)
{
    return functionMangledName(name, functionSuffix(params));
}

std::string functionMangledName(const BuiltinFunction& f);
std::string functionMangledName(const FunctionDef& f);
std::string functionMangledName(const FunctionRef& f);

}
}
#endif /* SL_AST_FUNCTIONMANGLEDNAME_HPP */
