@echo off

set "DEFAULT_BIN_PATH=C:/msys64/mingw64/bin"
echo Enter bin path for your MinGW64 installation
set /p "USER_BIN_PATH=(Type or press Enter to use default: %DEFAULT_BIN_PATH%): "

if "%USER_BIN_PATH%"=="" (
    set "USER_BIN_PATH=%DEFAULT_BIN_PATH%"
)

echo.

set "DEFAULT_LIB_PATH=C:/msys64/mingw64/lib"
echo Enter bin path for your MinGW64 installation
set /p "USER_LIB_PATH= (Type or press Enter to use default: %DEFAULT_LIB_PATH%): "

if "%USER_LIB_PATH%"=="" (
    set "USER_LIB_PATH=%DEFAULT_LIB_PATH%"
)

echo.

set "DEFAULT_INC_PATH=C:/msys64/mingw64/include"
echo Enter include path for your MinGW64 installation
set /p "USER_INC_PATH= (Type or press Enter to use default: %DEFAULT_INC_PATH%): "

if "%USER_INC_PATH%"=="" (
    set "USER_INC_PATH=%DEFAULT_INC_PATH%"
)

echo.

echo Selected /bin path: %USER_BIN_PATH%
echo Selected /lib path: %USER_LIB_PATH%
echo Selected /include path: %USER_INC_PATH%

cd wincon

echo .
echo Building PDCurses...

make -f Makefile WIDE=Y DLL=Y

echo .
echo Copying pdcurses.dll to %USER_BIN_PATH%...

copy "pdcurses.dll" "%USER_BIN_PATH%"

echo .
echo Renaming pdcurses.a to libpdcurses.a...

ren "pdcurses.a" "libpdcurses.a"

echo .
echo Copying libpdcurses.a to %USER_LIB_PATH%...

copy "libpdcurses.a" "%USER_LIB_PATH%"

cd ../

echo .
echo Copying curses.h to %USER_INC_PATH%...

copy "curses.h" "%USER_INC_PATH%"

echo .
echo Copying panel.h to %USER_INC_PATH%...

copy "panel.h" "%USER_INC_PATH%"

pause