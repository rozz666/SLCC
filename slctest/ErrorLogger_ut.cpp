#include <tut/tut.hpp> 
#include <sl/ErrorLogger.hpp>

namespace tut
{

struct ErrorLogger_Test_data
{
	sl::ErrorLogger el;

	ErrorLogger_Test_data() : el("") { }
};

bool operator==(const sl::err::Message& left, const sl::err::Message& right)
{
	return
		left.pos.column == right.pos.column && left.pos.line == right.pos.line &&
		left.id == right.id && left.text == right.text;
}

typedef test_group<ErrorLogger_Test_data> tg;
typedef tg::object object;
tg ErrorLogger_group("Error logger");

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
	set_test_name("Ctor");

	const std::string fname = "test.sl";

	sl::ErrorLogger el(fname);

	ensure_equals("filename", el.filename(), fname);
}

template <>
template <>
void object::test<3>()
{
	set_test_name("Copy ctor");

	sl::err::Message msg(sl::FilePosition(2, 3), "abcd", "texttext");

	el << sl::err::Message(msg);

	ensure_equals("size", el.errors().size(), 1u);
	ensure("message", el.errors().front() == msg);
}

template <>
template <>
void object::test<4>()
{
	set_test_name("Output format");

	sl::err::Message msg(sl::FilePosition(2, 3), "abcd", "texttext");

	const std::string fname = "test.sl";

	sl::ErrorLogger el(fname);

	el << sl::err::Message(msg);

	std::ostringstream os, mos;

	el.print(os);

	mos << fname << "(" << msg.pos.line << ", " << msg.pos.column << "): error " << msg.id << ": " << msg.text << std::endl;

	ensure_equals("message", os.str(), mos.str());
}

}