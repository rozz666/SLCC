#include <sl/test.hpp>

sl::test::TestSuite<1> suite("Basic tests");

template <>
void sl::test::TestSuite<1>::run()
{
    using namespace sl::err;

    test("Arithmetic", "arithmetic.sl")
        .match("1 4 5 2 9 5 3 3", "5\n3\n45\n1\n")
        .match("9 8 2 0 4 8 9 2", "17\n2\n32\n4\n");

    test("Empty", "empty.sl")
        .expect(module_declaration_missing(at(1, 1)));
}