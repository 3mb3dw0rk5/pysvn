setlocal
if exist testroot-03 rmdir /s /q testroot-03
mkdir testroot-03
subst b: /d
subst b: %CD%\testroot-03

mkdir b:\configdir
python test_callbacks.py
endlocal
