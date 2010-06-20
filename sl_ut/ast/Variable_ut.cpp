#include <tut/tut.hpp>
#include <sl/ast/Variable.hpp>

namespace tut
{

struct Variable_Test_data
{
};

typedef test_group<Variable_Test_data> tg;
typedef tg::object object;
tg Variable_group("ast.Variable");

template <>
template <>
void object::test<1>()
{
    std::string name("x");
    sl::FilePosition pos("xxx", 3, 4);
    sl::ast::Variable v(name, pos, sl::int_, false);

    ensure_equals("name", v.name(), name);
    ensure_equals("pos", v.pos(), pos);
    ensure_equals("type", v.type(), sl::int_);
    ensure_equals("ref", v.ref(), false);
}

}
