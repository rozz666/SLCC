#!/usr/bin/bash
if [ ! "$#" = "2" ]
then
    echo "usage: make_ut.sh <name> <desc>"

    exit 1
fi

cat ut_template | sed -e "s/\$(NAME)/$1/" | sed -e "s/\$(DESC)/$2/" > "$1_ut.cpp"