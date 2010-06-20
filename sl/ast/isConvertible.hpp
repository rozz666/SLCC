#ifndef SL_AST_ISCONVERTIBLE_HPP
#define SL_AST_ISCONVERTIBLE_HPP

#include <sl/basicTypes.hpp>

namespace sl
{
namespace ast
{

inline bool isConvertible(BasicType from, BasicType to)
{
    return
        (from == to) ||
        (to == float_ && from == int_) ||
        (to == int_ && from == float_);
}

}
}
#endif /* SL_AST_ISCONVERTIBLE_HPP */
