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

template <>
template <>
void object::test<2>()
{
    sl::ast::Variable v1("x", sl::FilePosition("a", 1, 2), sl::int_, false);
    sl::ast::Variable v2("y", sl::FilePosition("a", 2, 2), sl::int_, false);
    sl::ast::Variable v3("x", sl::FilePosition("a", 1, 2), sl::float_, false);
    sl::ast::Variable v4("x", sl::FilePosition("a", 1, 2), sl::int_, true);
    sl::ast::Variable v5(v1);

    ensure("v1 < v2", v1 < v2);
    ensure_not("v2 < v1", v2 < v1);
    ensure_not("v1 < v3", v1 < v3);
    ensure_not("v3 < v1", v3 < v1);
    ensure_not("v1 < v4", v1 < v4);
    ensure_not("v4 < v1", v4 < v1);
    ensure_not("v1 < v5", v1 < v5);
    ensure_not("v5 < v1", v5 < v1);
    ensure("v1 == v1", v1 == v1);
    ensure_not("v1 == v2", v1 == v2);
}

}
