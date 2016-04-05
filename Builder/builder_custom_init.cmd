@echo off

if not "%1" == "" set PY_VER=%1
if not "%2" == "" set SVN_VER_MAJ_MIN=%4
if not "%3" == "" set BUILD_ARCH=%3

if "%PY_VER%" == "" set PY_VER=2.7
if "%SVN_VER_MAJ_MIN%" == "" set SVN_VER_MAJ_MIN=1.9
if "%BUILD_ARCH%" == "" set BUILD_ARCH=Win32

if /i "%BUILD_ARCH%" EQU "win32" set BUILD_ARCH=Win32
if /i "%BUILD_ARCH%" EQU "win64" set BUILD_ARCH=Win64

set SVN_VER_MAJ_DASH_MIN=%SVN_VER_MAJ_MIN:.=-%
set PY_MAJ=%PY_VER:~0,1%
set PY_MIN=%PY_VER:~2%

rem Save CWD
pushd .

rem for compiler version information see
rem see https://docs.python.org/3.4/using/windows.html section 3.6
rem and https://docs.python.org/3.5/using/windows.html section 3.7
if "%PY_VER%" == "2.7" set VC_VER=9.0
if "%PY_VER%" == "3.3" set VC_VER=9.0
if "%PY_VER%" == "3.4" set VC_VER=9.0
if "%PY_VER%" == "3.5" set VC_VER=14.0

if exist ..\..\ReleaseEngineering\win32-vc-%VC_VER%\software-versions-%SVN_VER_MAJ_MIN%.cmd (
    echo Setting up compiler VC %VC_VER%
    pushd ..\..\ReleaseEngineering\win32-vc-%VC_VER%
    call software-versions-%SVN_VER_MAJ_MIN%.cmd %BUILD_ARCH%
    popd
) else (
    echo No compiler setup found
    pause
)

set PYCXX=%BUILDER_TOP_DIR%\Import\pycxx-%PYCXX_VER%
set PY=c:\python%PY_MAJ%%PY_MIN%.%BUILD_ARCH%
if not exist %PY%\python.exe set PY=c:\python%PY_MAJ%%PY_MIN%

set PYTHONPATH=%BUILDER_TOP_DIR%\Source
set PYTHON=%PY%\python.exe

rem Need python and SVN on the path
PATH %PY%;%SVN_BIN%;%PATH%

rem prove the python version selected works
python -c "import sys;print( 'Info: Python Version %%s' %% sys.version )"
echo SVN Version:
svn --version --quiet

rem restore original CWD
popd
