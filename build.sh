#!/bin/sh

set -xe

gcc -g main.c -o raytracer -lm -DDEBUG=1

./raytracer
feh ./output.ppm
