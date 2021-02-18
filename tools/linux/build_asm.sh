#!/bin/sh

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

NC='\033[0m' # No Color
RED='\033[1;91m'
ERROR="${RED}Error${NC}"


DEBUG_FORMAT=
PLATFORM=
PREFIX=

if [ "$ARCH" == "x86" ]
then

DEBUG_FORMAT=dwarf
PLATFORM=elf32
PREFIX="_"

elif [ "$ARCH" == "x64" ]
then

DEBUG_FORMAT=dwarf
PLATFORM=elf64
PREFIX=""

else

echo "$ERROR: wrong architecture set <$ARCH>"
exit 1

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
