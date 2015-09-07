setlocal
set PYVER=33
set SVNVER=1.7.14
set SVNVER=1.8.11
c:\python%PYVER%.win32\python setup.py configure --verbose %* --platform=win32 --pycxx-dir=%USERPROFILE%\wc\svn\PyCxx --svn-root-dir=c:\BuildRoot\Win32-MSVC14.0-%SVNVER%\svn-win32-%SVNVER%
endlocal
