@echo off
@prompt $P$S$G
@rem
@rem   test-09.cmd
@rem       test annotate, annotate2 and status2
@rem

@echo WorkDir: %BUILDER_TOP_DIR%
@echo PYTHON: %PYTHON%
@echo Username: %USERNAME%

setlocal
set PYTHONPATH=%BUILDER_TOP_DIR%\Source;%BUILDER_TOP_DIR%\Examples\Client
set PYSVN=%PYTHON% %BUILDER_TOP_DIR%\Examples\Client\svn_cmd.py --pysvn-testing 01.01.00 --config-dir b:\configdir
echo Info: PYSVN CMD %PYSVN%
call :cmd_shell mkdir testroot-09
call :cmd_shell subst b: %CD%\testroot-09
call :cmd_shell cd /d b:\

call :cmd_shell mkdir tmp
set TMPDIR=b:\tmp

call :cmd_shell svnadmin create b:\repos

echo Info: Setup - mkdir
call :cmd_pysvn mkdir file:///b:/repos/trunk -m "test-09 add trunk"

echo Info: Setup - checkout wc1
call :cmd_pysvn checkout file:///b:/repos/trunk b:\wc1
call :cmd_shell cd /d b:\wc1

echo Info: Setup - add files and folders

call :cmd_pysvn mkdir folder1

call :cmd_createfile folder1\file-a.txt test add file 1

call :cmd_pysvn add folder1\file-a.txt

call :cmd_pysvn checkin -m "commit added files"

call :cmd_appendfile folder1\file-a.txt test add line 2
call :cmd_pysvn checkin -m "add line 2"

call :cmd_pysvn diff folder1\file-a.txt >b:\diff-1.patch

call :cmd_pysvn annotate folder1\file-a.txt
call :cmd_pysvn annotate2 folder1\file-a.txt

echo Info: propset_local
call :cmd_pysvn propset_local svn:eol native folder1/file-a.txt
call :cmd_pysvn proplist --verbose folder1/file-a.txt
call :cmd_pysvn checkin -m "eol is native"

call :cmd_pysvn propdel_local svn:eol folder1/file-a.txt
call :cmd_pysvn proplist --verbose folder1/file-a.txt
call :cmd_pysvn checkin -m "remove eol"

call :cmd_pysvn "propset_remote" test-case 09 file:///b:/repos/trunk/folder1/file-a.txt --revision 5 -m "set custom prop"

call :cmd_pysvn proplist --verbose file:///b:/repos/trunk/folder1/file-a.txt

call :cmd_pysvn "propdel_remote" test-case file:///b:/repos/trunk/folder1/file-a.txt --revision 6 -m "set custom prop"

call :cmd_pysvn proplist --verbose file:///b:/repos/trunk/folder1/file-a.txt

call :cmd_pysvn update

call :cmd_shell cd /d folder1
call :cmd_createfile unversioned.txt empty

call :cmd_pysvn status2
call :cmd_pysvn status2 --verbose
call :cmd_pysvn status2 --verbose --quiet

call :cmd_pysvn lock file-a.txt
call :cmd_pysvn status2
call :cmd_pysvn status2 --verbose
call :cmd_pysvn status2 --verbose --quiet

goto :eof

:cmd_shell
    echo.
    echo Info: CMD %*
    %*
    exit /b

:cmd_createfile
    set FILENAME=%1
    shift
    echo Info: Create File %FILENAME% - %1 %2 %3 %4 %5 %6 %7 %8 %9
    call :cmd__echo %1 %2 %3 %4 %5 %6 %7 %8 %9 >%FILENAME%
    exit /b

:cmd_appendfile
    set FILENAME=%1
    shift
    echo Info: Append File %FILENAME% - %1 %2 %3 %4 %5 %6 %7 %8 %9
    call :cmd__echo %1 %2 %3 %4 %5 %6 %7 %8 %9 >>%FILENAME%
    exit /b

:cmd__echo
    echo %*
    exit /b

:cmd_pysvn
    echo. 1>&2
    echo Info: PYSVN CMD %* 1>&2
    %PYSVN% %*
    exit /b

endlocal
