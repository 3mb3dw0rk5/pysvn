setlocal
if exist testroot rmdir /s /q testroot
mkdir testroot
subst b: /d
subst b: %CD%\testroot

mkdir b:\configdir
python test_callbacks.py
endlocal
