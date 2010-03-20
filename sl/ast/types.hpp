#ifndef SL_AST_TYPES_HPP
#define SL_AST_TYPES_HPP

#include <cassert>
#include <sl/def.hpp>
#include <sl/basicTypes.hpp>

namespace sl
{
namespace ast
{

inline const char *typeSuffix(BasicType type)
{
    switch (type)
    {
        case int_: return "i";
        case float_: return "f";
        case bool_: return "b";
    }

    assert(!"Invalid type");

    return "";
}

inline const char *typeName(BasicType type)
{
    switch (type)
    {
        case int_: return "int";
        case float_: return "float";
        case bool_: return "bool";
        case void_: return "void";
    }

    assert(!"Invalid type");

    return "";
}

inline unsigned short typeSize(BasicType type)
{
    switch (type)
    {
        case int_: return sizeof(int_t);
        case float_: return sizeof(float_t);
        case bool_: return sizeof(int_t);
    }

    assert(!"Bad type!");

    return 0;
}

}
}
#endif /* SL_AST_TYPES_HPP */
