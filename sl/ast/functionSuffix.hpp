#ifndef SL_AST_FUNCTIONSUFFIX_HPP
#define SL_AST_FUNCTIONSUFFIX_HPP

#include <string>
#include <memory>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/range/iterator.hpp>
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
functionSuffix(const Container& params)
{
    std::string suffix;

    BOOST_FOREACH(const Variable& v, params)
    {
        suffix += typeSuffix(v.type());
    }

    return suffix;
}

template <typename Range>
inline 
typename boost::enable_if<
    boost::is_same<
        typename boost::remove_const<
            typename std::iterator_traits<typename boost::range_iterator<Range>::type>::value_type
        >::type,
        sl::BasicType
    >,
    std::string
>::type
functionSuffix(const Range& params)
{
    std::string suffix;

    BOOST_FOREACH(const sl::BasicType& t, params)
    {
        suffix += typeSuffix(t);
    }

    return suffix;
}


}
}
#endif /* SL_AST_FUNCTIONSUFFIX_HPP */
