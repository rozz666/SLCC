#include <sl/test.hpp>

sl::test::TestSuite<6> suite("new delete");

template <>
void sl::test::TestSuite<6>::run()
{
    test("new delete", "newdelete.sl")
        .expect((values, 1, 5, 2, 5.0, 3, 1, 1, 6));
}