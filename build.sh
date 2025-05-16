#!/bin/bash

# usage function
usage() {
    echo "usage: $0 [--debug] [-cc || --compiler gcc] [-h || --help] [--extra 'extra cc tags'] [--no-shared] [--no-static]"
    exit 1
}

# origin
ORIGIN=$(pwd)

# default values
DEBUG=0
CC="gcc -Wformat=0"
EXTRA=""
NO_SHARED=0
NO_STATIC=0

# parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug) DEBUG=1; shift ;;
        -cc) CC="$2"; shift 2 ;;
        --compile) CC="$2"; shift 2 ;;
        --extra) EXTRA="$2"; shift 2 ;;
        --no-shared) NO_SHARED=1; shift ;;
        --no-static) NO_STATIC=1; shift ;;
        --help) usage ;;
        -h) usage ;;
        *) echo "unknown option: $1"; usage ;;
    esac
done

if [[ $DEBUG -eq 1 ]]; then
    DEBUGARGS='-g'
    echo "debug mode enabled"
else
    DEBUGARGS=""
fi

echo "compiler: $CC"

rm -rf build
mkdir -p build

if [[ $NO_SHARED -eq 0 ]]; then # also build a shared library
    echo "building shared library"
    $CC src/br.c -o build/libbr.so -lbruter -shared -fPIC -O3 -Iinclude $DEBUGARGS $EXTRA
fi

if [[ $NO_STATIC -eq 0 ]]; then
    echo "building static library"
    $CC src/br.c -o build/libbr.a -c -lbruter -O3 -Iinclude $DEBUGARGS $EXTRA
fi