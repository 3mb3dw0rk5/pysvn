@call "c:\Program Files\Microsoft Visual Studio\VC98\Bin\VCVARS32.BAT"
@echo on

rem figure out where we are
for %%I in ( %0\..\.. ) do set WORKDIR=%%~fI

set PY_MAJ=2
if not "%1" == "" set PY_MAJ=%1
set PY_MIN=3
if not "%2" == "" set PY_MIN=%2

set SVNVER=1.0.0
set RAPIDSVNVER=0.5.0
set PYCXX=%WORKDIR%\Import\pycxx_5_2_1
set OPENSSL=L:\subversion\win32-%SVNVER%\openssl-0.9.7c
set SUBVERSION=L:\subversion\win32-%SVNVER%\subversion-%SVNVER%
set RAPIDSVN=L:\subversion\win32-%SVNVER%\rapidsvn-%RAPIDSVNVER%
set PY=c:\python%PY_MAJ%%PY_MIN%
set PYLIB=python%PY_MAJ%%PY_MIN%

rem Need python and DB4 on the path
PATH %PY%;%SUBVERSION%\db4-win32\bin;%SUBVERSION%\Release\subversion\svnadmin;%PATH%

rem prove the python version selected
python -c "import sys;print 'Info: Python Version',sys.version"
