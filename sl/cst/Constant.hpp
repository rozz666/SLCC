#ifndef SL_CST_CONSTANT_HPP
#define SL_CST_CONSTANT_HPP

#include <boost/variant.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <sl/def.hpp>
#include <sl/FilePosition.hpp>

namespace sl
{
namespace cst
{

struct Constant
{
    typedef boost::variant<int_t, float_t, bool> Value;
    FilePosition pos;
    Value value;
};

}
}

BOOST_FUSION_ADAPT_STRUCT(
    sl::cst::Constant,
    (sl::FilePosition, pos)
    (sl::cst::Constant::Value, value)
)

#endif /* SL_CST_CONSTANT_HPP */
