#!/bin/bash

if [ "$1" == "" ] ; then
    echo "verze?"
    exit 1
fi

apack edmonds-$1.zip edmonds.cpp Makefile edmonds.pdf \
graf-dot.c vygeneruj-obrazky.sh \
3x5.in.png adam.in.png c6.in.png c7.in.png k4.in.png k5.in.png kvet.in.png maly-kvet.in.png osmicka.in.png volna.in.png \
3x5.in adam.in c6.in c7.in k4.in k5.in kvet.in maly-kvet.in osmicka.in volna.in
