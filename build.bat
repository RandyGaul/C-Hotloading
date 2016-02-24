@echo off

if not defined DEV_ENV_DIR (
	call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64
)
set DEV_ENV_DIR= ???

set CFLAGS= -Zi -nologo
set LFLAGS= -incremental:no user32.lib kernel32.lib

if not exist .\bin mkdir .\bin
pushd .\bin

del *.pdb > NUL 2> NUL

REM game dll
echo "WAITING FOR PDB ..." > lock.tmp
cl %CFLAGS% ..\src\game.cpp -LD /link -PDB:game_%random%.pdb %LFLAGS%
del lock.tmp

REM platform exe
cl %CFLAGS% ..\src\main.cpp /link %LFLAGS%

popd

echo Done!
