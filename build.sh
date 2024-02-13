#!/bin/sh

set -xe

CC="${CXX:-cc}"

gcc kic.c -o kic -Wall -ggdb -O3 -std=c11 -pedantic
