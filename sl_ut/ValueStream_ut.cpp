#include <tut/tut.hpp>
#include <sl/test/ValueStream.hpp>
#include <sl/def.hpp>

namespace tut
{

struct ValueStream_Test_data
{
    sl::test::ValueStream vs;
};

typedef test_group<ValueStream_Test_data> tg;
typedef tg::object object;
tg ValueStream_group("Value stream");

template <>
template <>
void object::test<1>()
{
    sl::int_t val1 = 1;
    sl::int_t val2 = 2;
    vs.put(val1);
    vs.put(val2);

    ensure_equals("val1", vs.get<sl::int_t>(), val1);
    ensure_equals("val2", vs.get<sl::int_t>(), val2);
}

template <>
template <>
void object::test<2>()
{
    ensure_not("no value", vs.get<sl::int_t>().is_initialized());
}

template <>
template <>
void object::test<3>()
{
    sl::float_t val1 = 1.1f;
    sl::float_t val2 = 2.2f;
    vs.put(val1);
    vs.put(val2);

    ensure_equals("val1", vs.get<sl::float_t>(), val1);
    ensure_equals("val2", vs.get<sl::float_t>(), val2);
}

template <>
template <>
void object::test<4>()
{
    sl::float_t val1 = 1.1f;
    sl::int_t val2 = 2;
    vs.put(val1);
    vs.put(val2);

    ensure_equals("val1", vs.get<sl::float_t>(), val1);
    ensure_equals("val2", vs.get<sl::int_t>(), val2);
}

template <>
template <>
void object::test<5>()
{
    sl::float_t val = 1.1f;
    vs.put(val);

    ensure_not("no value", vs.get<sl::int_t>().is_initialized());
    ensure_equals("value", vs.get<sl::float_t>(), val);
}

}
