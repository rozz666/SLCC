#include <tut/tut.hpp> 
#include <sl/ast/Assignment.hpp>

namespace tut
{

struct Assignment_Test_data
{
};

typedef test_group<Assignment_Test_data> tg;
typedef tg::object object;
tg Assignment_group("ast.Assignment");

template <>
template <>
void object::test<1>()
{
    sl::ast::Variable v("asia", sl::FilePosition("a", 2, 3), sl::int_, false);
    sl::ast::Variable v2("xxx", sl::FilePosition("b", 1, 1), sl::int_, true);
    sl::ast::Assignment a(v, v2);

    ensure("var", a.var() == v);
    ensure("expr type", a.expr().type() == typeid(v2));
    ensure("expr", boost::get<sl::ast::Variable>(a.expr()) == v2);
}

}
