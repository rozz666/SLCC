#include <tut/tut.hpp>
#include <boost/lexical_cast.hpp>
#include <sl/test/ValueList.hpp>

namespace tut
{

struct ValueList_Test_data
{
};

typedef test_group<ValueList_Test_data> tg;
typedef tg::object object;
tg ValueList_group("Value list wrapper");

template <>
template <>
void object::test<1>()
{
    const sl::test::ValueList values;

    ensure_equals(boost::lexical_cast<std::string>(static_cast<sl::test::ValueStream& >(values, 10)), "10");
    ensure_equals(boost::lexical_cast<std::string>(static_cast<sl::test::ValueStream& >(values, 1.2, 2.3, 3)), "1.2 2.3 3");
}

}
