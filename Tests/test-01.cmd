setlocal
set PYTHONPATH=%CD%\..\Extension\pyds;%CD%\..\Examples
set PYSVN=c:\python23\python %CD%\..\Examples\svn_cmd.py
set PATH=c:\svn;%PATH%
if exist testroot rmdir /s /q testroot
mkdir testroot
subst b: /d
subst b: %CD%\testroot

mkdir b:\repo
svnadmin create b:\repos


%PYSVN% mkdir file:///b:/repos/trunk -m "test-01 add trunk"
%PYSVN% ls file:///b:/repos -v -R
endlocal
