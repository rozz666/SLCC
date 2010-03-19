#ifndef SL_VM_DEF_HPP
#define SL_VM_DEF_HPP

#include <cstdint>

namespace sl
{
namespace vm
{

typedef std::uint8_t byte;

static_assert(sizeof(byte) == 1, "byte is 8-bit");

typedef std::uint32_t CodeAddr;
typedef std::uint32_t DataAddr;
typedef std::int32_t BPAddr;

typedef std::int32_t int_t;
typedef float float_t;

}
}
#endif /* SL_VM_DEF_HPP */
