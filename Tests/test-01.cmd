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

rem mkdir
%PYSVN% mkdir file:///b:/repos/trunk -m "test-01 add trunk"
%PYSVN% mkdir file:///b:/repos/trunk/test -m "test-01 add test"
rem ls
%PYSVN% ls file:///b:/repos -v -R
rem checkout
%PYSVN% checkout file:///b:/repos/trunk b:\wc1
dir b:\wc1 /s
cd b:\wc1\test
rem add
echo test add file >file1.txt
%PYSVN% add file1.txt
%PYSVN% checkin -m "commit added file"

rem update - get a new wc that will update
%PYSVN% checkout file:///b:/repos/trunk b:\wc2

rem - checkin a mod from wc1
echo line 2 >>b:\wc1\test\file1.txt
%PYSVN% checkin -m "commit modifled file"

rem update
%PYSVN% update file:///b:/repos/trunk b:\wc2
endlocal
