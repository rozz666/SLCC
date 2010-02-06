#ifndef SL_PARSEAST_HPP
#define SL_PARSEAST_HPP

#include <iosfwd>
#include <boost/optional.hpp>
#include "ast.hpp"

namespace sl
{

boost::optional<ast::Module> parseFile(std::istream& is);

}

#endif /* SL_PARSEAST_HPP */