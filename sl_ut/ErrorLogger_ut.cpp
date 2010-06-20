#include <tut/tut.hpp> 
#include <sl/ErrorLogger.hpp>

namespace tut
{

struct ErrorLogger_Test_data
{
    sl::ErrorLogger el;
};

bool operator==(const sl::err::Message& left, const sl::err::Message& right)
{
    return
        left.pos == right.pos &&
        left.id == right.id &&
        left.text == right.text;
}

typedef test_group<ErrorLogger_Test_data> tg;
typedef tg::object object;
tg ErrorLogger_group("ErrorLogger");

template <>
template <>
void object::test<1>()
{
    set_test_name("Empty");
    std::ostringstream os;

    ensure("empty", el.errors().empty());
    el.print(os);
    ensure_equals(os.str().length(), 0u);
}

template <>
template <>
void object::test<2>()
{
    set_test_name("Copy ctor");

    sl::err::Message msg(sl::FilePosition("abc", 2, 3), "abcd", "texttext");

    el << sl::err::Message(msg);

    ensure_equals("size", el.errors().size(), 1u);
    ensure("message", el.errors().front() == msg);
}

template <>
template <>
void object::test<3>()
{
    set_test_name("Output format");

    sl::err::Message msg(sl::FilePosition("test.sl", 2, 3), "abcd", "texttext");

    el << sl::err::Message(msg);

    std::ostringstream os, mos;

    el.print(os);

    mos << msg.pos.file << "(" << msg.pos.line << ", " << msg.pos.column << "): error " << msg.id << ": " << msg.text << std::endl;

    ensure_equals("message", os.str(), mos.str());
}

}