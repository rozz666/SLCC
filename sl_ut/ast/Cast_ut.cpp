#include <tut/tut.hpp> 
#include <sl/ast/Cast.hpp>

namespace tut
{

struct Cast_Test_data
{
};

typedef test_group<Cast_Test_data> tg;
typedef tg::object object;
tg Cast_group("ast.Cast");

template <>
template <>
void object::test<1>()
{
    sl::ast::Variable v("asia", sl::FilePosition("a", 2, 3), sl::int_, false);
    sl::FilePosition pos("a", 1, 2);
    sl::ast::Cast c(pos, v, sl::float_);

    ensure("pos", c.pos() == pos);
    ensure("expr type", c.expr().type() == typeid(v));
    ensure("expr", boost::get<sl::ast::Variable>(c.expr()) == v);
    ensure("type", c.type() == sl::float_);
}

}
