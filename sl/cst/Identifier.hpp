#ifndef SL_CST_IDENTIFIER_HPP
#define SL_CST_IDENTIFIER_HPP

#include <string>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/FilePosition.hpp>

namespace sl
{
namespace cst
{

struct Identifier
{
    FilePosition pos;
    std::string str;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::Identifier,
    (sl::FilePosition, pos)
    (std::string, str)
)

#endif /* SL_CST_IDENTIFIER_HPP */
