setlocal
set PYTHONPATH=%WORKDIR%\Source;%WORKDIR%\Examples\Client
set PYSVN=%PY%\python %WORKDIR%\Examples\Client\svn_cmd.py
if exist testroot rmdir /s /q testroot
mkdir testroot
subst b: /d
subst b: %CD%\testroot

mkdir b:\repo
echo %PATH%
svnadmin create b:\repos

%PYSVN% mkdir file:///b:/repos/trunk -m "test-01 add trunk"
%PYSVN% ls file:///b:/repos -v -R
endlocal
