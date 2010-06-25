#include <tut/tut.hpp> 
#include <sl/ast/BuiltinFunction.hpp>
#include <sl/ast/functionSuffix.hpp>

namespace tut
{

struct BuiltinFunction_Test_data
{
};

typedef test_group<BuiltinFunction_Test_data> tg;
typedef tg::object object;
tg BuiltinFunction_group("ast.BuiltinFunction");

template <>
template <>
void object::test<1>()
{
    sl::ast::BuiltinFunction bf("f", sl::int_);

    ensure_equals("name", bf.name(), "f");
    ensure("type", bf.type() == sl::int_);
    ensure_equals("suffix", bf.suffix(), sl::ast::functionSuffix(bf.argTypes()));
    ensure_equals("no params", bf.argTypes().size(), 0u);
}

template <>
template <>
void object::test<2>()
{
    sl::ast::BuiltinFunction bf("f", sl::float_, sl::int_);

    ensure_equals("name", bf.name(), "f");
    ensure("type", bf.type() == sl::int_);
    ensure_equals("suffix", bf.suffix(), sl::ast::functionSuffix(bf.argTypes()));
    ensure_equals("param count", bf.argTypes().size(), 1u);
    ensure("param type", bf.argTypes()[0] == sl::float_);
}

template <>
template <>
void object::test<3>()
{
    sl::ast::BuiltinFunction bf("f", sl::float_, sl::int_, sl::int_);

    ensure_equals("name", bf.name(), "f");
    ensure("type", bf.type() == sl::int_);
    ensure_equals("suffix", bf.suffix(), sl::ast::functionSuffix(bf.argTypes()));
    ensure_equals("param count", bf.argTypes().size(), 2u);
    ensure("param type", bf.argTypes()[0] == sl::float_);
    ensure("param type", bf.argTypes()[1] == sl::int_);
}

}
