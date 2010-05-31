#include <sl/test.hpp>

sl::test::TestSuite<1> suite("Functional tests");

template <>
void sl::test::TestSuite<1>::run()
{
    using namespace sl::err;

    test("Arithmetic", "arithmetic.sl")
        .match("1 4 5 2 9 5 3 3", "5\n3\n45\n1\n")
        .match("9 8 2 0 4 8 9 2", "17\n2\n32\n4\n");

    test("Empty", "empty.sl")
        .expect(module_declaration_missing(at(1, 1)));

    test("Double variable declaration", "doublevdecl.sl")
        .expect(variable_already_declared(at(3, 14), "a"))
        .expect(variable_earlier_declaration(at(3, 7), "a"))
        .expect(variable_already_declared(at(11, 9), "a"))
        .expect(variable_earlier_declaration(at(10, 9), "a"));

    test("Double function declaration", "doublefdecl.sl")
        .expect(function_already_declared(at(8, 1), "f(int)"))
        .expect(function_earlier_declaration(at(3, 1), "f(int)"))
        .expect(function_already_declared(at(13, 1), "f(int)"))
        .expect(function_earlier_declaration(at(3, 1), "f(int)"));

    test("new delete", "newdelete.sl")
        .match("", "1\n5\n2\n5\n3\n1\n1\n6\n");

    test("bad delete", "baddelete.sl")
        .expect(unknown_identifier(at(9, 16), "a"))
        .expect(unknown_identifier(at(19, 16), "a"))
        .expect(unknown_identifier(at(27, 5), "a"));
}