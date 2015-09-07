setlocal
set PYVER=27
set SVNVER=1.6.17
set SVNVER=1.7.14
set SVNVER=1.8.11
c:\python%PYVER%.win32\python setup.py configure --verbose %* --platform=win32 --pycxx-dir=c:\wc\svn\PyCxx --svn-root-dir=c:\BuildRoot\Win32-MSVC90-%SVNVER%\svn-win32-%SVNVER%
endlocal
