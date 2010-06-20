#include <tut/tut.hpp>
#include <sl/ast/VariableDecl.hpp>

namespace tut
{

struct VariableDecl_Test_data
{
};

typedef test_group<VariableDecl_Test_data> tg;
typedef tg::object object;
tg VariableDecl_group("ast.VariableDecl");

template <>
template <>
void object::test<1>()
{
    using namespace sl;

    std::string name("x");
    FilePosition pos("y", 1, 2);
    ast::VariableDecl vd(name, pos, sl::int_);
    
    ensure_equals("var name", vd.var().name(), name);
    ensure_equals("var pos", vd.var().pos(), pos);
    ensure_equals("var type", vd.var().type(), sl::int_);
    ensure_equals("var ref", vd.var().ref(), false);
    ensure_not("no expr", vd.expr().is_initialized());
}

template <>
template <>
void object::test<2>()
{
}

template <>
template <>
void object::test<3>()
{
}

}
