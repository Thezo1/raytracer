#!/bin/sh

set -xe

gcc -g main.c -o raytracer -lm

./raytracer
