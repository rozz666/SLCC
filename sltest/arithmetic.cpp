#include <sl/test.hpp>

sl::test::TestSuite<3> suite("Aritmetic");

template <>
void sl::test::TestSuite<3>::run()
{
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
}