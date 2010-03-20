#ifndef SL_AST_FUNCTIONSUFFIX_HPP
#define SL_AST_FUNCTIONSUFFIX_HPP

#include <string>
#include <memory>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/foreach.hpp>
#include <sl/ast/Variable.hpp>
#include <sl/ast/types.hpp>

namespace sl
{
namespace ast
{

template <typename Container>
inline 
typename boost::enable_if<
    boost::is_same<typename boost::remove_reference<typename Container::reference>::type, std::shared_ptr<Variable> >,
    std::string
>::type
functionSuffix(const Container& params)
{
    std::string suffix;

    BOOST_FOREACH(const std::shared_ptr<Variable>& v, params)
    {
        suffix += typeSuffix(v->type());
    }

    return suffix;
}



}
}
#endif /* SL_AST_FUNCTIONSUFFIX_HPP */
