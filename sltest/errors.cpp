#include <sl/test.hpp>

sl::test::TestSuite<2> suite("Error tests");

template <>
void sl::test::TestSuite<2>::run()
{
    using namespace sl::err;

    test("Empty", "empty.sl")
        .expect(module_declaration_missing(at("empty.sl", 1, 1)));

    test("Whitespace", "whitespace.sl")
        .expect(module_declaration_missing(at("whitespace.sl", 1, 1)));

    test("Double variable declaration", "doublevdecl.sl")
        .expect(variable_already_declared(at("doublevdecl.sl", 3, 14), "a"))
        .expect(variable_earlier_declaration(at("doublevdecl.sl", 3, 7), "a"))
        .expect(variable_already_declared(at("doublevdecl.sl", 11, 9), "a"))
        .expect(variable_earlier_declaration(at("doublevdecl.sl", 10, 9), "a"));

    test("Double function declaration", "doublefdecl.sl")
        .expect(function_already_declared(at("doublefdecl.sl", 8, 1), "f(int)"))
        .expect(function_earlier_declaration(at("doublefdecl.sl", 3, 1), "f(int)"))
        .expect(function_already_declared(at("doublefdecl.sl", 13, 1), "f(int)"))
        .expect(function_earlier_declaration(at("doublefdecl.sl", 3, 1), "f(int)"))
        .expect(function_already_declared(at("doublefdecl.sl", 15, 1), "swap(int, int)"))
        .expect(function_already_declared(at("doublefdecl.sl", 17, 1), "swap(int, int)"))
        .expect(function_already_declared(at("doublefdecl.sl", 19, 1), "geti()"))
        .expect(function_already_declared(at("doublefdecl.sl", 23, 1), "r(int)"))
        .expect(function_earlier_declaration(at("doublefdecl.sl", 21, 1), "r(int)"));

    test("bad delete", "baddelete.sl")
        .expect(unknown_identifier(at("baddelete.sl", 9, 16), "a"))
        .expect(unknown_identifier(at("baddelete.sl", 19, 16), "a"))
        .expect(unknown_identifier(at("baddelete.sl", 27, 5), "a"));

    test("no main", "nomain.sl")
        .expect(missing_main(at("nomain.sl", 1, 1)));

    test("bad main", "badmain.sl")
        .expect(invalid_main_type(at("badmain.sl", 3, 1)));

    test("bad overloading", "badoverloading.sl")
        .expect(function_not_found(at("badoverloading.sl", 17, 5), "f(bool)"))
        .expect(ambiguous_call_to_function(at("badoverloading.sl", 18, 5), "f(float, float)"))
        .expect(ambiguous_call_to_function(at("badoverloading.sl", 19, 5), "f(float, int)"))
        .expect(function_not_found(at("badoverloading.sl", 20, 5), "f(bool, int)"))
        .expect(function_not_found(at("badoverloading.sl", 21, 5), "f(bool, float)"))
        .expect(function_not_found(at("badoverloading.sl", 22, 5), "f(int, bool)"));
}