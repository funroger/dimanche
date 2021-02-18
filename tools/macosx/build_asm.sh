#!/bin/bash

# $1 path to DIMANCHE root
# $2 architecture (win32/x86/x64)
# $3 path to an .asm file
# $4 output dir

clear

DIMANCHE=$1
ARCH=$2
SRC_FILE=$3
FULL_FILE_NAME=${SRC_FILE##*/}
FILE_NAME=${FULL_FILE_NAME%%.asm}
DST_FILE=$4$FILE_NAME.o
NASM=nasm

DEBUG_FORMAT=
PLATFORM=
PREFIX=

if [ "$ARCH" == "x86" ]
then
DEBUG_FORMAT=dwarf
PLATFORM=macho32
PREFIX="_"
fi

if [ "$ARCH" == "x64" ]
then
DEBUG_FORMAT=dwarf
PLATFORM=macho64
PREFIX=""
fi

# for printing listing add -E

$NASM \
  -f $PLATFORM \
  -g$DEBUG_FORMAT \
  -I$DIMANCHE \
  -o $DST_FILE \
  --prefix $PREFIX \
  -w+all \
  -w-macro-params-legacy \
  -Xgnu \
  $SRC_FILE
