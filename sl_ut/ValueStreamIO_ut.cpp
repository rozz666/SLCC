#include <tut/tut.hpp>
#include <sl/test/ValueStreamIO.hpp>

namespace tut
{

struct ValueStreamIO_Test_data
{
};

typedef test_group<ValueStreamIO_Test_data> tg;
typedef tg::object object;
tg ValueStreamIO_group("value stream I/O");

template <>
template <>
void object::test<1>()
{
    sl::test::ValueStream is;
    sl::test::ValueStream os;

    sl::test::ValueStreamIO vio(is, os);

    vio.putInt(10);
    
    ensure_equals("value", os.get<sl::int_t>(), 10);
}

template <>
template <>
void object::test<2>()
{
    sl::test::ValueStream is;
    sl::test::ValueStream os;

    sl::test::ValueStreamIO vio(is, os);

    vio.putFloat(10.5f);
    
    ensure_equals("value", os.get<sl::float_t>(), 10.5f);
}

template <>
template <>
void object::test<3>()
{
    sl::test::ValueStream is;
    sl::test::ValueStream os;

    is.put<sl::int_t>(10);
    is.put<sl::int_t>(20);

    sl::test::ValueStreamIO vio(is, os);
    
    ensure_equals("value 1", vio.getInt(), 10);
    ensure_equals("value 2", vio.getInt(), 20);
}

template <>
template <>
void object::test<4>()
{
    sl::test::ValueStream is;
    sl::test::ValueStream os;

    is.put<sl::float_t>(10.0f);
    is.put<sl::float_t>(20.5f);

    sl::test::ValueStreamIO vio(is, os);
    
    ensure_equals("value 1", vio.getFloat(), 10.0f);
    ensure_equals("value 2", vio.getFloat(), 20.5f);
}

}
