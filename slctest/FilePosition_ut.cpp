#include <tut/tut.hpp> 
#include <sl/FilePosition.hpp>

namespace tut
{

struct FilePosition_Test_data
{
};

typedef test_group<FilePosition_Test_data> tg;
typedef tg::object object;
tg FilePosition_group("File position");

template <>
template <>
void object::test<1>()
{
	set_test_name("ctors");

	sl::FilePosition fp;
	
	ensure_equals("default line", fp.line, 0);
	ensure_equals("default column", fp.column, 0);

	sl::FilePosition fp2(56, 78);

	ensure_equals("line", fp2.line, 56);
	ensure_equals("column", fp2.column, 78);
}

template <>
template <>
void object::test<2>()
{
	set_test_name("operator==");

	const sl::FilePosition fp(123, 456);

	ensure("equal", sl::FilePosition(123, 456) == sl::FilePosition(123, 456));
	ensure_not("unequal 1", sl::FilePosition(5123, 274) == sl::FilePosition(123, 456));
	ensure_not("unequal 2", sl::FilePosition(5123, 274) == sl::FilePosition(123, 274));
	ensure_not("unequal 3", sl::FilePosition(5123, 274) == sl::FilePosition(5123, 456));
}

template <>
template <>
void object::test<3>()
{
	set_test_name("operator!=");

	const sl::FilePosition fp(123, 456);

	ensure_not("equal", sl::FilePosition(123, 456) != sl::FilePosition(123, 456));
	ensure("unequal 1", sl::FilePosition(5123, 274) != sl::FilePosition(123, 456));
	ensure("unequal 2", sl::FilePosition(5123, 274) != sl::FilePosition(123, 274));
	ensure("unequal 3", sl::FilePosition(5123, 274) != sl::FilePosition(5123, 456));
}

template <>
template <>
void object::test<4>()
{
	set_test_name("operator<");

	ensure_not("not less 1", sl::FilePosition(123, 456) < sl::FilePosition(123, 456));
	ensure_not("not less 2", sl::FilePosition(123, 456) < sl::FilePosition(123, 450));
	ensure_not("not less 2", sl::FilePosition(123, 456) < sl::FilePosition(120, 450));
	ensure("less 1", sl::FilePosition(123, 456) < sl::FilePosition(123, 457));
	ensure("less 2", sl::FilePosition(123, 456) < sl::FilePosition(124, 457));
}

}
