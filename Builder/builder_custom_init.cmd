@call "c:\Program Files\Microsoft Visual Studio\VC98\Bin\VCVARS32.BAT"
@echo on

rem figure out where we are
for %%I in ( %0\..\.. ) do set WORKDIR=%%~fI

set PY_MAJ=2
if not "%1" == "" set PY_MAJ=%1
set PY_MIN=3
if not "%2" == "" set PY_MIN=%2

rem Save CWD
pushd .

rem in development the version info can be found
rem otherwise the builder will have run it already
set COMPILER=msvc60
if "%PY_MIN%" == "4" set COMPILER=msvc71

if exist ..\..\ReleaseEngineering\win32-%COMPILER%\software-versions.cmd call ..\..\ReleaseEngineering\win32-%COMPILER%\software-versions.cmd

set PYCXX=%WORKDIR%\Import\pycxx_%PYCXX_VER%
set OPENSSL=%TARGET%\openssl-%OPENSSL_VER%
set SUBVERSION=%TARGET%\subversion-%SVN_VER%
set APR=%TARGET%\httpd-%HTTPD_VER%\srclib
set PY=c:\python%PY_MAJ%%PY_MIN%
set PYLIB=python%PY_MAJ%%PY_MIN%
set PYTHONPATH=%WORKDIR%\Source

rem Need python and DB4 on the path
PATH %PY%;%SUBVERSION%\db4-win32\bin;%SUBVERSION%\Release\subversion\svnadmin;%APR%\apr\Release;%APR%\apr-util\Release;%APR%\apr-iconv\Release;%PATH%

rem prove the python version selected
python -c "import sys;print 'Info: Python Version',sys.version"

rem restore original CWD
popd
