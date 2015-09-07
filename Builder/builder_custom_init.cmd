@echo off

set PY_MAJ=2
if not "%1" == "" set PY_MAJ=%1
set PY_MIN=7
if not "%2" == "" set PY_MIN=%2
if not "%3" == "" set BUILD_TYPE=%3
if not "%4" == "" set SVN_VER_MAJ_MIN=%4


if "%SVN_VER_MAJ_MIN%" == "" set /p SVN_VER_MAJ_MIN="Build Version (1.8): "
if "%SVN_VER_MAJ_MIN%" == "" set SVN_VER_MAJ_MIN=1.8

if "%SVN_VER_MAJ_MIN%" == "1.4" set SVN_VER_MAJ_DASH_MIN=1-4
if "%SVN_VER_MAJ_MIN%" == "1.5" set SVN_VER_MAJ_DASH_MIN=1-5
if "%SVN_VER_MAJ_MIN%" == "1.6" set SVN_VER_MAJ_DASH_MIN=1-6
if "%SVN_VER_MAJ_MIN%" == "1.7" set SVN_VER_MAJ_DASH_MIN=1-7
if "%SVN_VER_MAJ_MIN%" == "1.8" set SVN_VER_MAJ_DASH_MIN=1-8
if "%SVN_VER_MAJ_MIN%" == "1.9" set SVN_VER_MAJ_DASH_MIN=1-9

if "%SVN_VER_MAJ_DASH_MIN%" == "" (
    echo "Error: SVN version %SVN_VER_MAJ_MIN% not supported %SVN_VER_MAJ_DASH_MIN%"
    pause
    goto end
)

rem Save CWD
pushd .

set VC_VER=14.0

if exist ..\..\ReleaseEngineering\win32-vc-%VC_VER%\software-versions-%SVN_VER_MAJ_MIN%.cmd (
    echo Setting up compiler VC %VC_VER%
    pushd ..\..\ReleaseEngineering\win32-vc-%VC_VER%
    call software-versions-%SVN_VER_MAJ_MIN%.cmd off
    popd
    ) else (
    echo No compiler setup found
    pause
    )

set PYCXX=%BUILDER_TOP_DIR%\Import\pycxx-%PYCXX_VER%
set PY=c:\python%PY_MAJ%%PY_MIN%.win32
if not exist %PY%\python.exe set PY=c:\python%PY_MAJ%%PY_MIN%

set PYTHONPATH=%BUILDER_TOP_DIR%\Source
set PYTHON=%PY%\python.exe

rem Need python and SVN on the path
PATH %PY%;%SVN_BIN%;%PATH%

rem prove the python version selected
python -c "import sys;print( 'Info: Python Version %%s' %% sys.version )"
echo SVN Version:
svn --version --quiet

rem restore original CWD
popd
:end
