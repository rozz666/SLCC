#ifndef SL_AST_CONSTANT_HPP
#define SL_AST_CONSTANT_HPP

#include <boost/variant.hpp>
#include <sl/def.hpp>
#include <sl/basicTypes.hpp>
#include <sl/FilePosition.hpp>

namespace sl
{
namespace ast
{

class Constant
{
public:

    typedef boost::variant<int_t, float_t, bool> Value;

    Constant(const FilePosition& pos, const Value& value) : pos_(pos), value_(value) { }

    FilePosition pos() const { return pos_; }
    Value value() const { return value_; }
    BasicType type() const;

private:

    FilePosition pos_;
    Value value_;
};

}
}
#endif /* SL_AST_CONSTANT_HPP */
