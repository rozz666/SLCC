#include <sl/test.hpp>

sl::test::TestSuite<1> suite("Functional tests");

template <>
void sl::test::TestSuite<1>::run()
{
    using namespace sl::err;

    test("Identity", "identity.sl")
        .match("10", "10\n")
        .match("15", "15\n")
        .match("-39", "-39\n");

    test("Plus", "plus.sl")
        .match("10", "10\n")
        .match("0", "0\n")
        .match("-10", "-10\n");

    test("Minus", "neg.sl")
        .match("10", "-10\n")
        .match("0", "0\n")
        .match("-10", "10\n");

    test("Add", "add.sl")
        .match("1423 6245", "7668\n")
        .match("-36 23", "-13\n")
        .match("-55 -66", "-121\n");

    test("Sub", "sub.sl")
        .match("1423 6245", "-4822\n")
        .match("-36 23", "-59\n")
        .match("-55 -66", "11\n");

    test("Mul", "mul.sl")
        .match("1423 6245", "8886635\n")
        .match("-36 23", "-828\n")
        .match("-55 -66", "3630\n");

    test("Div", "div.sl")
        .match("123 5", "24\n")
        .match("-36 3", "-12\n")
        .match("-55 -6", "9\n");

    test("Mod", "mod.sl")
        .match("123 5", "3\n")
        .match("-37 3", "-1\n")
        .match("-55 -6", "-1\n");

    test("Logical Not", "lnot.sl")
        .match("0", "1\n")
        .match("1", "0\n");

    test("Logical And", "land.sl")
        .match("0 0", "0\n")
        .match("1 0", "0\n")
        .match("1 1", "1\n")
        .match("0 1", "0\n");

    test("Logical Or", "lor.sl")
        .match("0 0", "0\n")
        .match("1 0", "1\n")
        .match("1 1", "1\n")
        .match("0 1", "1\n");

    test("Swap", "swaptest.sl")
        .match("20 50 6.25 -0.5 1 0", "50\n20\n-0.5\n6.25\n0\n1\n")
        .match("-3 10 -0.125 0.125 0 1", "10\n-3\n0.125\n-0.125\n1\n0\n");

    test("Passing by reference", "references.sl")
        .match("10 10.5 1", "10\n10.5\n1\n")
        .match("-22 -0.5 0", "-22\n-0.5\n0\n");

    test("while loop", "whiletest.sl")
        .match("50", "50\n")
        .match("0", "0\n");

    test("if", "iftest.sl")
        .match("-5 5", "5\n5\n5\n5\n")
        .match("10 20", "20\n20\n20\n20\n");

    test("Empty", "empty.sl")
        .expect(module_declaration_missing(at(1, 1)));

    test("Whitespace", "whitespace.sl")
        .expect(module_declaration_missing(at(1, 10)));

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