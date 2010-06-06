#include <tut/tut.hpp> 
#include <sl/err/Message.hpp>
#include <boost/lexical_cast.hpp>

namespace tut
{

struct Message_Test_data
{
};

typedef test_group<Message_Test_data> tg;
typedef tg::object object;
tg Message_group("Message");

template <>
template <>
void object::test<1>()
{
    set_test_name("ctors");

    sl::err::Message msg(sl::FilePosition(123, 456), "X123", "This is an error");

    ensure_equals("pos", msg.pos, sl::FilePosition(123, 456));
    ensure_equals("id", msg.id, "X123");
    ensure_equals("text", msg.text, "This is an error");

}

template <>
template <>
void object::test<2>()
{
    set_test_name("operator<<");

    std::ostringstream out, tmp;
    sl::err::Message msg(sl::FilePosition(543, 6543), "ABCD", "Error message");

    out << msg;
    tmp << msg.pos << ": error " << msg.id << ": " << msg.text;

    ensure_equals("text", out.str(), tmp.str());
}

}
