setlocal
if not "%1" == "" set PYVER=%1
if "%PYVER%" == "" set /P PYVER="Python version (Mm): "
if not EXIST c:\python%PYVER%.win32\python.exe (
    echo Error: Unknown python version
    exit /b 2
    )

if not "%2" == "" set SVNVER=%2
if "%SVNVER%" == "" set /P SVNVER="SVN version (M.m.P): "

if not EXIST c:\BuildRoot\Win32-MSVC14.0-%SVNVER%\svn-win32-%SVNVER%\bin\svn.exe (
    echo Error: Unknown SVN version
    exit /b 2
    )

c:\python%PYVER%.win32\python setup.py configure --verbose --platform=win32 --pycxx-dir=%USERPROFILE%\wc\svn\PyCxx --svn-root-dir=c:\BuildRoot\Win32-MSVC14.0-%SVNVER%\svn-win32-%SVNVER%
if ERRORLEVEL 1 goto :EOF
nmake clean
if ERRORLEVEL 1 goto :EOF
nmake
if ERRORLEVEL 1 goto :EOF
cd ..\Tests
if ERRORLEVEL 1 goto :EOF
nmake clean
if ERRORLEVEL 1 goto :EOF
nmake
endlocal
