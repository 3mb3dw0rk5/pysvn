setlocal
set PYTHONPATH=.
if exist testroot rmdir /s /q testroot
mkdir testroot
c:\python23\python %WORKDIR%\Tests\thread_tests.py 20 http://torment.chelsea.private/svn/repos1/Latest/
endlocal
