@prompt $P$S$G
@echo WorkDir: %WORKDIR%
@echo PYTHON: %PY%\python

setlocal
set PYTHONPATH=%WORKDIR%\Source;%WORKDIR%\Examples\Client
set PYSVN=%PY%\python %WORKDIR%\Examples\Client\svn_cmd.py --config-dir b:\configdir
mkdir testroot-01
subst b: %CD%\testroot-01
cd /d b:\

svnadmin create b:\repos

rem mkdir
%PYSVN% mkdir file:///b:/repos/trunk -m "test-01 add trunk"
%PYSVN% mkdir file:///b:/repos/trunk/test -m "test-01 add test"

rem ls
%PYSVN% ls file:///b:/repos -v -R

rem checkout
%PYSVN% checkout file:///b:/repos/trunk b:\wc1
dir b:\wc1 /s /b /a-h
cd /d b:\wc1\test

rem add
echo test add file 1 >file1.txt
echo test add file 2 >file2.txt
echo test add file 3 >file3.txt
echo test add file 4 >file4.txt
echo test add file 5 >file5.txt
%PYSVN% add file1.txt
%PYSVN% add file2.txt
%PYSVN% add file3.txt
%PYSVN% add file4.txt
%PYSVN% add file5.txt
%PYSVN% checkin -m "commit added files"

rem update - get a new wc that will update
%PYSVN% checkout file:///b:/repos/trunk b:\wc2

rem - checkin a mod from wc1
echo line 2 >>b:\wc1\test\file1.txt
%PYSVN% checkin -m "commit modified file"

rem update
%PYSVN% update b:\wc2

rem the rest in lexical order

rem annotate
%PYSVN% annotate b:\wc2\test\file1.txt

rem cat
%PYSVN% cat -r head file:///b:/repos/trunk/test/file1.txt

rem cleanup

rem copy
%PYSVN% mkdir file:///b:/repos/tags -m "test-01 add tags"
echo tag the trunk | %PYSVN% copy file:///b:/repos/trunk file:///b:/repos/tags/version1
%PYSVN% ls -v file:///b:/repos/tags
%PYSVN% copy b:\wc2\test\file1.txt b:\wc2\test\file1b.txt
%PYSVN% checkin b:\wc2 -m "copy test"

rem diff
echo new line >>b:\wc2\test\file1b.txt
%PYSVN% diff b:\wc2

rem export
%PYSVN% export file:///b:/repos/trunk/test b:\export1
dir /s /b b:\export1

rem import

rem info
%PYSVN% info b:\wc2\test
%PYSVN% info b:\wc2\test\file1.txt

rem log
%PYSVN% log b:\wc2

rem ls
%PYSVN% ls file:///b:/repos/trunk/test
%PYSVN% ls -v file:///b:/repos/trunk/test
%PYSVN% ls b:\wc2\test
%PYSVN% ls -v b:\wc2\test

rem merge

rem mkdir - done above

rem move
echo move url test |%PYSVN% move file:///b:/repos/trunk/test/file2.txt file:///b:/repos/trunk/test/file2b.txt 
%PYSVN% move b:\wc2\test\file3.txt b:\wc2\test\file3b.txt
%PYSVN% checkin b:\wc2 -m "move wc test"


rem status
echo file 4 is changing >>b:\wc1\test\file4.txt
%PYSVN% checkin b:\wc1 -m "change wc1 for status -u to detect"

%PYSVN% status b:\wc2
%PYSVN% status --verbose b:\wc2
%PYSVN% status --show-updates b:\wc2
%PYSVN% status --show-updates --verbose b:\wc2
%PYSVN% update
%PYSVN% status --show-updates b:\wc2
%PYSVN% status --show-updates --verbose b:\wc2
%PYSVN% checkin b:\wc2 -m "prop change"

rem propdel
cd /d b:\wc2\test
%PYSVN% propset test:prop1 del_me file4.txt
%PYSVN% proplist -v file4.txt
%PYSVN% propdel test:prop1 file4.txt
%PYSVN% proplist -v file4.txt

rem propget
%PYSVN% propget svn:eol-style file4.txt

rem proplist - see above

rem propset
cd /d b:\wc2\test
%PYSVN% proplist -v file4.txt
%PYSVN% propset svn:eol-style native file4.txt
%PYSVN% proplist -v file4.txt

rem remove
cd /d b:\wc2\test
%PYSVN% remove file5.txt
%PYSVN% status

rem resolved
echo conflict in file4 yes >>b:\wc1\test\file4.txt
echo conflict in file4 no >>b:\wc2\test\file4.txt
%PYSVN% checkin b:\wc1\test -m "make a conflict part 1"
%PYSVN% update b:\wc2\test
%PYSVN% status
copy b:\wc2\test\file4.txt.mine b:\wc2\test\file4.txt
%PYSVN% resolved b:\wc2\test\file4.txt
%PYSVN% checkin b:\wc2\test\file4.txt -m "resolve a confict part 2"

rem revert
%PYSVN% revert file5.txt
%PYSVN% status

rem revproplist
%PYSVN% revproplist file:///b:/repos/trunk

rem revpropget
%PYSVN% revpropget svn:log file:///b:/repos/trunk
%PYSVN% revpropget no_such_prop file:///b:/repos/trunk

rem revpropset
%PYSVN% revpropset svn:log "Hello world" file:///b:/repos/trunk

rem revpropdel
%PYSVN% revpropdel svn:log file:///b:/repos/trunk

rem status - see above

rem relocate
mkdir b:\root
move b:\repos b:\root
%PYSVN% info b:\wc1
%PYSVN% relocate file:///b:/repos/trunk file:///b:/root/repos/trunk b:\wc1
%PYSVN% info b:\wc1
%PYSVN% info b:\wc2
%PYSVN% relocate file:///b:/repos/trunk file:///b:/root/repos/trunk b:\wc2
%PYSVN% info b:\wc2

rem switch
%PYSVN% info b:\wc2
%PYSVN% switch b:\wc2 file:///b:/root/repos/tags/version1
%PYSVN% info b:\wc2

rem update - see above

endlocal
