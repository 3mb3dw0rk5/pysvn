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
if exist ..\..\ReleaseEngineering\win32\software-versions.cmd call ..\..\ReleaseEngineering\win32\software-versions.cmd

rem set PYCXX=%WORKDIR%\Import\pycxx_%PYCXX_VER%
set PYCXX=L:\wc\PyCXX\CXX
set OPENSSL=%BUILDROOT_DIR%\win32-%SVN_VER%\openssl-%OPENSSL_VER%
set SUBVERSION=%BUILDROOT_DIR%\win32-%SVN_VER%\subversion-%SVN_VER%
set APR=%BUILDROOT_DIR%\win32-%SVN_VER%\httpd-%HTTPD_VER%\srclib
set PY=c:\python%PY_MAJ%%PY_MIN%
set PYLIB=python%PY_MAJ%%PY_MIN%

rem Need python and DB4 on the path
PATH %PY%;%SUBVERSION%\db4-win32\bin;%SUBVERSION%\Release\subversion\svnadmin;%APR%\apr\Release;%APR%\apr-util\Release;%APR%\apr-iconv\Release;%PATH%

rem prove the python version selected
python -c "import sys;print 'Info: Python Version',sys.version"

rem restore original CWD
popd
