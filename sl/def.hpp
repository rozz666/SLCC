#ifndef SL_DEF_HPP
#define SL_DEF_HPP

#include <cstdint>

namespace sl
{

typedef std::uint8_t byte;

static_assert(sizeof(byte) == 1, "byte is 8-bit");

typedef std::int32_t int_t;
typedef std::uint32_t uint_t;
typedef float float_t;

static_assert(sizeof(float_t) == sizeof(int_t), "float must be 32-bit");

}
#endif /* SL_DEF_HPP */
