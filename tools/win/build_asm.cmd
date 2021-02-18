@echo off

REM %0 path to this .cmd file
REM %1 platform (win32/x86/x64)
REM %2 path to an .asm file
REM %3 output dir

SET SRC_FILE=%2
SET DST_FILE=%3\%~n2.obj
SET DIMANCHE=%~dp0..\..
SET NASM=%DIMANCHE%\tools\win\nasm.exe

SET DEBUG_FORMAT=
SET PLATFORM=
SET PREFIX=


IF /I [%1]==[win32] (GOTO X86)
IF /I [%1]==[x86] (GOTO X86)
IF /I [%1]==[x64] (GOTO X86_64)


:X86
SET DEBUG_FORMAT=cv8
SET PLATFORM=win32
SET PREFIX="_"
GOTO DO


:X86_64
SET DEBUG_FORMAT=cv8
SET PLATFORM=win64
SET PREFIX=""
GOTO DO

# for printing listing add -E

:DO
%NASM% ^
  -f %PLATFORM% ^
  -g%DEBUG_FORMAT% ^
  -I%DIMANCHE% ^
  -o %DST_FILE% ^
  --prefix %PREFIX% ^
  -w+all ^
  -Xvc ^
  %SRC_FILE%
