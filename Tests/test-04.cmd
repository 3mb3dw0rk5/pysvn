@prompt $P$S$G$S
@echo WorkDir: %WORKDIR%
@echo PYTHON: %PY%\python

setlocal
set PYTHONPATH=%WORKDIR%\Source;%WORKDIR%\Examples\Client
set PYSVN=%PY%\python %WORKDIR%\Examples\Client\svn_cmd.py --config-dir b:\configdir
mkdir testroot-04
subst b: %CD%\testroot-04
cd /d b:\

svnadmin create b:\repos

rem mkdir
%PYSVN% mkdir file:///b:/repos/trunk -m "test-01 add trunk"
%PYSVN% mkdir file:///b:/repos/trunk/test -m "test-01 add test"

rem Install hooks
rem echo echo svnlook info %%1 -t %%2 ^>b:\test_1.output >b:\repos\hooks\pre-commit.cmd
rem echo svnlook info %%1 -t %%2 ^>^>b:\test_1.output >>b:\repos\hooks\pre-commit.cmd
rem echo echo svnlook changed %%1 -t %%2 ^>^>b:\test_1.output >>b:\repos\hooks\pre-commit.cmd
rem echo svnlook changed %%1 -t %%2 ^>^>b:\test_1.output >>b:\repos\hooks\pre-commit.cmd
echo echo %PY%\python %WORKDIR%\Tests\test_04_pre_commit_test_1.py %%* ^>^>b:\test_1.output >>b:\repos\hooks\pre-commit.cmd
echo %PY%\python %WORKDIR%\Tests\test_04_pre_commit_test_1.py %%* ^>^>b:\test_1.output >>b:\repos\hooks\pre-commit.cmd


rem Add one dir
%PYSVN% mkdir file:///b:/repos/trunk/test/a -m "pre-commit test 1"
type b:\test_1.output

rem Add two files
%PYSVN% co file:///b:/repos/trunk/test b:\wc
echo file1 ROOT >b:\wc\file1.txt
echo file1 A >b:\wc\a\file1.txt

%PYSVN% add b:\wc\file1.txt
%PYSVN% add b:\wc\a\file1.txt
%PYSVN% ci -m "Add two files" b:\wc
type b:\test_1.output

rem Mod one file Mod one prop

echo file1 ROOT ln 2 >b:\wc\file1.txt
%PYSVN% propset svn:eol-style native b:\wc\a\file1.txt
%PYSVN% ci -m "Mod one file Mod one prop" b:\wc
type b:\test_1.output

endlocal
