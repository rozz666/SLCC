#ifndef SL_AST_STRPARAMETERS_HPP
#define SL_AST_STRPARAMETERS_HPP

#include <string>
#include <sstream>
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

//TODO: replace with range
template <typename Container>
inline 
typename boost::enable_if<
    boost::is_same<typename boost::remove_reference<typename Container::reference>::type, Variable>,
    std::string
>::type
strParameters(const Container& params)
{
    std::ostringstream os;
    bool comma = false;

    os << "(";

    BOOST_FOREACH(const Variable& v, params)
    {
        if (comma) os << ", ";
        comma = true;
        os << typeName(v.type());
    }

    os << ")";

    return os.str();
}

//TODO: replace with range
template <typename Container>
inline 
typename boost::enable_if<
    boost::is_same<typename boost::remove_reference<typename Container::reference>::type, BasicType>,
    std::string
>::type
strParameters(const Container& params)
{
    std::ostringstream os;
    bool comma = false;

    os << "(";

    BOOST_FOREACH(BasicType v, params)
    {
        if (comma) os << ", ";
        comma = true;
        os << typeName(v);
    }

    os << ")";

    return os.str();
}

}
}
#endif /* SL_AST_STRPARAMETERS_HPP */
