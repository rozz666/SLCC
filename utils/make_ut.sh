#!/usr/bin/bash
if [ ! "$#" = "2" ]; then
    echo "usage: make_ut.sh <name> <desc>"
    exit 1
fi
NAME="$1"
DESC="$2"
CODE="#include <tut/tut.hpp> 

namespace tut
{

struct ${NAME}_Test_data
{
};

typedef test_group<${NAME}_Test_data> tg;
typedef tg::object object;
tg ${NAME}_group(\"$DESC\");

template <>
template <>
void object::test<1>()
{
}

template <>
template <>
void object::test<2>()
{
}

template <>
template <>
void object::test<3>()
{
}

}"

echo "$CODE" > "${NAME}_ut.cpp"