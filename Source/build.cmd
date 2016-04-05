setlocal
set PY_VER=%1
set SVN_VER_MAJ_MIN=%2
set BUILD_ARCH=%3

call ..\Builder\builder_custom_init.cmd

python setup.py configure --verbose --platform=%BUILD_ARCH% --pycxx-dir=%USERPROFILE%\wc\svn\PyCxx --distro-dir=%TARGET%\dist

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
