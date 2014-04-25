@ECHO OFF

REM git describe --always

rmdir /S /Q bin
rmdir /S /Q lib
rmdir /S /Q win_build32
rmdir /S /Q win_build64


REM __CSI_HAS_OPENSSL__

ECHO ===== CMake for 64-bit ======
mkdir win_build64 
cd win_build64
del CMakeCache.txt
cmake -G "Visual Studio 12 Win64" ..
cd ..
if NOT %errorlevel% == 0 goto errorexit
goto cleanexit

:errorexit
set returncode=%errorlevel%
echo There was an error during CMake (error code %returncode%).
exit /b %returncode%

:cleanexit

