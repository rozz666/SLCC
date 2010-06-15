#include <tut/tut.hpp>
#include <sstream>
#include <sl/vm/StdStreamIO.hpp>

namespace tut
{

struct StdStreamIO_Test_data
{
};

typedef test_group<StdStreamIO_Test_data> tg;
typedef tg::object object;
tg StdStreamIO_group("std stream I/O");

template <>
template <>
void object::test<1>()
{
    std::istringstream is;
    std::ostringstream os;

    sl::vm::StdStreamIO sio(is, os);

    sio.putInt(10);
    
    ensure_equals("value", os.str(), "10\n");
}

template <>
template <>
void object::test<2>()
{
    std::istringstream is;
    std::ostringstream os;

    sl::vm::StdStreamIO sio(is, os);

    sio.putFloat(10.5f);

    std::ostringstream tmp;
    tmp << sl::float_t(10.5f) << std::endl;
    
    ensure_equals("value", os.str(), tmp.str());
}

template <>
template <>
void object::test<3>()
{
    std::istringstream is("10 20");
    std::ostringstream os;

    sl::vm::StdStreamIO sio(is, os);
    
    ensure_equals("value 1", sio.getInt(), 10);
    ensure_equals("value 2", sio.getInt(), 20);
}

template <>
template <>
void object::test<4>()
{
    std::istringstream is("10 20.5");
    std::ostringstream os;

    sl::vm::StdStreamIO sio(is, os);
    
    ensure_equals("value 1", sio.getFloat(), 10.0f);
    ensure_equals("value 2", sio.getFloat(), 20.5f);
}

}
