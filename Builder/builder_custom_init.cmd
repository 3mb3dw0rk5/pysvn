@call "c:\Program Files\Microsoft Visual Studio\VC98\Bin\VCVARS32.BAT"
@echo on

rem figure out where we are
for %%I in ( %0\..\.. ) do set WORKDIR=%%~fI

set SVNVER=0.36.0
set PYCXX=%WORKDIR%\Import\pycxx_5_2_0
set OPENSSL=L:\subversion\win32-%SVNVER%\openssl-0.9.7c
set SUBVERSION=L:\subversion\win32-%SVNVER%\subversion-%SVNVER%
set RAPIDSVN=L:\subversion\win32-%SVNVER%\rapidsvn
set PY=c:\python23
set PYLIB=python23

rem Need python and DB4 on the path
PATH %PY%;%SUBVERSION%\db4-win32\bin;%SUBVERSION%\Release\subversion\svnadmin;%PATH%

rem prove the python version selected
python -c "import sys;print 'Info: Python Version',sys.version"
