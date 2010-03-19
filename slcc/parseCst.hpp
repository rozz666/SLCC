#ifndef SL_PARSEAST_HPP
#define SL_PARSEAST_HPP

#include <iosfwd>
#include <boost/optional.hpp>
#include "ErrorLogger.hpp"
#include "cst.hpp"

namespace sl
{

boost::optional<cst::Module> parseFile(std::istream& is, ErrorLogger& errorLogger);

}

#endif /* SL_PARSEAST_HPP */