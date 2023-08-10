#!/bin/sh

set -xe

gcc -g main.c -o raytracer -lm `sdl2-config --cflags --libs` -DPPM_RENDER=1
