@echo off

REM %1 path to DIMANCHE root
REM %2 architecture (win32/x86/x64)
REM %3 path to an .asm file
REM %4 output dir

SET DIMANCHE=%1
SET ARCH=%2
SET SRC_FILE=%3
SET DST_FILE=%4\%~n3.obj
SET NASM=nasm

SET ERROR=Error

where %NASM% >NULL
IF NOT [0]==[%ERRORLEVEL%] (
echo %ERROR%: nasm not found. Please check if nasm is in the PATH environment variable
exit /b
)

SET DEBUG_FORMAT=cv8
SET PLATFORM=
SET PREFIX=

IF /I [%ARCH%]==[win32] (
SET ARCH=x86
)

IF /I [%ARCH%]==[x86_64] (
SET ARCH=x64
)

IF /I [%ARCH%]==[x86] (

SET PLATFORM=win32
SET PREFIX="_"

) ELSE IF /I [%ARCH%]==[x64] (

SET PLATFORM=win64
SET PREFIX=""

) ELSE (
echo %ERROR%: wrong architecture set "%ARCH%"
exit /b
)

REM for printing listing add -E

%NASM% ^
  -f %PLATFORM% ^
  -g%DEBUG_FORMAT% ^
  -I%DIMANCHE% ^
  -o %DST_FILE% ^
  --prefix %PREFIX% ^
  -w+all ^
  -w-macro-params-legacy ^
  -Xvc ^
  %SRC_FILE%
