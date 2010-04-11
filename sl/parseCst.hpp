#ifndef SL_PARSECST_HPP
#define SL_PARSECST_HPP

#include <iosfwd>
#include <boost/optional.hpp>
#include <sl/ErrorLogger.hpp>
#include <sl/cst.hpp>

namespace sl
{

boost::optional<cst::Module> parseFile(std::istream& is, ErrorLogger& errorLogger);

}

#endif /* SL_PARSECST_HPP */