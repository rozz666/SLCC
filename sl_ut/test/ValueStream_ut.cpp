#include <tut/tut.hpp>
#include <boost/lexical_cast.hpp>
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
tg ValueStream_group("test.ValueStream");

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

template <>
template <>
void object::test<6>()
{
    sl::test::ValueStream vs2;

    ensure("empty equal", vs == vs2);
    ensure_not("empty not unequal", vs != vs2);

    vs.put<sl::int_t>(1);
    vs2.put<sl::int_t>(1);

    ensure("equal", vs == vs2);
    ensure_not("not unequal", vs != vs2);

    vs.put<sl::int_t>(2);
    vs2.put<sl::float_t>(2.0f);

    ensure_not("not equal", vs == vs2);
    ensure("unequal", vs != vs2);
}

template <>
template <>
void object::test<7>()
{
    ensure_equals("empty text", boost::lexical_cast<std::string>(vs), "");

    vs.put<sl::int_t>(-20);
    ensure_equals(boost::lexical_cast<std::string>(vs), "-20");

    vs.put<sl::float_t>(5.5f);
    ensure_equals(boost::lexical_cast<std::string>(vs), "-20 5.5");
}

}
