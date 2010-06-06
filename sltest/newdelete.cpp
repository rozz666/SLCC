#include <sl/test.hpp>

sl::test::TestSuite<6> suite("new delete");

template <>
void sl::test::TestSuite<6>::run()
{
    test("new delete", "newdelete.sl")
        .match("", "1\n5\n2\n5\n3\n1\n1\n6\n");
}