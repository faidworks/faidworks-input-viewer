#!/bin/sh
set -e

cmake -B build_dir -S .
cmake --build build_dir
