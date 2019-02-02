:: For bulding SETPAL with Borland Turbo C 2.
@echo off

cd..

set OUTPUT_EXE=setpal.exe
set PATH=%PATH%;C:\TC\
tcc -1 -A -ms -e%OUTPUT_EXE% setpal.c
@if ERRORLEVEL 1 goto fail
copy %OUTPUT_EXE% bin\%OUTPUT_EXE% > NUL
del %OUTPUT_EXE%
del *.OBJ
goto done

:fail
echo BUILD FAILED.
:done

