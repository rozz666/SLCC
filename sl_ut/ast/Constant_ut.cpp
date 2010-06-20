#include <tut/tut.hpp> 
#include <sl/ast/Constant.hpp>

namespace tut
{

struct Constant_Test_data
{
};

typedef test_group<Constant_Test_data> tg;
typedef tg::object object;
tg Constant_group("ast.Constant");

template <>
template <>
void object::test<1>()
{
    sl::FilePosition pos("xxx", 1, 2);
    sl::int_t val(10);
    sl::ast::Constant c(pos, val);

    ensure_equals("pos", c.pos(), pos);
    ensure("value", c.value() == sl::ast::Constant::Value(val));
    ensure_equals("type", c.type(), sl::int_);
}

template <>
template <>
void object::test<2>()
{
    sl::FilePosition pos("xxx", 1, 2);
    sl::float_t val(10.0f);
    sl::ast::Constant c(pos, val);

    ensure_equals("pos", c.pos(), pos);
    ensure("value", c.value() == sl::ast::Constant::Value(val));
    ensure_equals("type", c.type(), sl::float_);
}

template <>
template <>
void object::test<3>()
{
    sl::FilePosition pos("xxx", 1, 2);
    bool val(false);
    sl::ast::Constant c(pos, val);

    ensure_equals("pos", c.pos(), pos);
    ensure("value", c.value() == sl::ast::Constant::Value(val));
    ensure_equals("type", c.type(), sl::bool_);
}

}
