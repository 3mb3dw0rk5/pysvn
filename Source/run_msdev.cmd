setlocal
call setup-msvc6
set PYCXX=L:\Latest\PyCXX
set SUBVERSION=L:\subversion\subversion-0.32.1
set SVNCPP=L:\subversion\rapidsvn\src\svncpp
set PY=c:\python23
set PYLIB=python23

PATH c:\svn;%PATH%
msdev pysvn.dsw

endlocal
