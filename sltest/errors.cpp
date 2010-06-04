#include <sl/test.hpp>

sl::test::TestSuite<2> suite("Error tests");

template <>
void sl::test::TestSuite<2>::run()
{
    using namespace sl::err;

    test("Empty", "empty.sl")
        .expect(module_declaration_missing(at(1, 1)));

    test("Whitespace", "whitespace.sl")
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
        .expect(function_earlier_declaration(at(3, 1), "f(int)"))
        .expect(function_already_declared(at(15, 1), "swap(int, int)"))
        .expect(function_already_declared(at(17, 1), "swap(int, int)"))
        .expect(function_already_declared(at(19, 1), "geti()"))
        .expect(function_already_declared(at(23, 1), "r(int)"))
        .expect(function_earlier_declaration(at(21, 1), "r(int)"));

    test("bad delete", "baddelete.sl")
        .expect(unknown_identifier(at(9, 16), "a"))
        .expect(unknown_identifier(at(19, 16), "a"))
        .expect(unknown_identifier(at(27, 5), "a"));

    test("no main", "nomain.sl")
        .expect(missing_main(at(3, 1)));

    test("bad main", "badmain.sl")
        .expect(invalid_main_type(at(3, 1)));
}