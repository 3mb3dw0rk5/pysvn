@echo off
rem figure out where we are
for %%I in ( %0\..\.. ) do set WORKDIR=%%~fI

set PY_MAJ=2
if not "%1" == "" set PY_MAJ=%1
set PY_MIN=4
if not "%2" == "" set PY_MIN=%2
set BUILD_TYPE=Release
if not "%3" == "" set BUILD_TYPE=%3

rem Save CWD
pushd .

rem in development the version info can be found
rem otherwise the builder will have run it already
set COMPILER=msvc60
if "%PY_MIN%" == "4" set COMPILER=msvc71
if "%PY_MIN%" == "5" set COMPILER=msvc71

if exist ..\..\ReleaseEngineering\win32-%COMPILER%\software-versions.cmd call ..\..\ReleaseEngineering\win32-%COMPILER%\software-versions.cmd off

set PYCXX=%WORKDIR%\Import\pycxx_%PYCXX_VER%
set OPENSSL=%TARGET%\openssl-%OPENSSL_VER%
set SUBVERSION=%TARGET%\subversion-%SVN_VER%
set APR=%SUBVERSION%
set PY=c:\python%PY_MAJ%%PY_MIN%
set PYLIB=python%PY_MAJ%%PY_MIN%
set PYTHONPATH=%WORKDIR%\Source
set PYTHON=%PY%\python.exe

rem Need python and DB4 on the path
PATH %PY%;%SUBVERSION%\db4-win\bin;%SUBVERSION%\db4-win\lib;%SUBVERSION%\svn-win32-libintl\bin;%SUBVERSION%\%BUILD_TYPE%\subversion\svnadmin;%SUBVERSION%\%BUILD_TYPE%\subversion\svnversion;%SUBVERSION%\%BUILD_TYPE%\subversion\clients\cmdline;%APR%\apr\%BUILD_TYPE%;%APR%\apr-util\%BUILD_TYPE%;%APR%\apr-iconv\%BUILD_TYPE%;%PATH%

rem prove the python version selected
python -c "import sys;print 'Info: Python Version',sys.version"

rem restore original CWD
popd
