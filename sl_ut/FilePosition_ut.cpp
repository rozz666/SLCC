#include <tut/tut.hpp> 
#include <sl/FilePosition.hpp>

namespace tut
{

struct FilePosition_Test_data
{
};

typedef test_group<FilePosition_Test_data> tg;
typedef tg::object object;
tg FilePosition_group("FilePosition");

template <>
template <>
void object::test<1>()
{
    set_test_name("ctors");

    sl::FilePosition fp;
    
    ensure_equals("default file", fp.file, "");
    ensure_equals("default line", fp.line, 0);
    ensure_equals("default column", fp.column, 0);

    sl::FilePosition fp2("test.sl", 56, 78);

    ensure_equals("file", fp2.file, "test.sl");
    ensure_equals("line", fp2.line, 56);
    ensure_equals("column", fp2.column, 78);
}

template <>
template <>
void object::test<2>()
{
    set_test_name("operator==");

    ensure("equal", sl::FilePosition("abc", 123, 456) == sl::FilePosition("abc", 123, 456));
    ensure_not("unequal 1", sl::FilePosition("abc", 5123, 274) == sl::FilePosition("abc", 123, 456));
    ensure_not("unequal 2", sl::FilePosition("abc", 5123, 274) == sl::FilePosition("abc", 123, 274));
    ensure_not("unequal 3", sl::FilePosition("abc", 5123, 274) == sl::FilePosition("abc", 5123, 456));
    ensure_not("unequal 4", sl::FilePosition("abc", 123, 456) == sl::FilePosition("xxx", 123, 456));
}

template <>
template <>
void object::test<3>()
{
    set_test_name("operator!=");

    ensure_not("equal", sl::FilePosition("abc", 123, 456) != sl::FilePosition("abc", 123, 456));
    ensure("unequal 1", sl::FilePosition("abc", 5123, 274) != sl::FilePosition("abc", 123, 456));
    ensure("unequal 2", sl::FilePosition("abc", 5123, 274) != sl::FilePosition("abc", 123, 274));
    ensure("unequal 3", sl::FilePosition("abc", 5123, 274) != sl::FilePosition("abc", 5123, 456));
    ensure("unequal 4", sl::FilePosition("abc", 123, 456) != sl::FilePosition("xxx", 123, 456));
}

template <>
template <>
void object::test<4>()
{
    set_test_name("operator<");

    ensure_not("not less 1", sl::FilePosition("abc", 123, 456) < sl::FilePosition("abc", 123, 456));
    ensure_not("not less 2", sl::FilePosition("abc", 123, 456) < sl::FilePosition("abc", 123, 450));
    ensure_not("not less 3", sl::FilePosition("abc", 123, 456) < sl::FilePosition("abc", 120, 450));
    ensure_not("not less 4", sl::FilePosition("zzz", 123, 456) < sl::FilePosition("abc", 123, 456));
    ensure("less 1", sl::FilePosition("abc", 123, 456) < sl::FilePosition("abc", 123, 457));
    ensure("less 2", sl::FilePosition("abc", 123, 456) < sl::FilePosition("abc", 124, 457));
    ensure("less 3", sl::FilePosition("abc", 123, 456) < sl::FilePosition("zzz", 123, 456));
}

template <>
template <>
void object::test<5>()
{
    set_test_name("operator<<");

    {
        std::ostringstream os;
        os << sl::FilePosition("abc", 123, 456);
        ensure_equals(os.str(), "abc(123, 456)");
    }
    {
        std::ostringstream os;
        os << sl::FilePosition("xxx", 67, 234);
        ensure_equals(os.str(), "xxx(67, 234)");
    }
}

}
