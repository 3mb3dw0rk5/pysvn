@prompt $P$S$G
@echo WorkDir: %BUILDER_TOP_DIR%
@echo PYTHON: %PYTHON%
@echo Username: %USERNAME%

setlocal
mkdir testroot-03
subst b: %CD%\testroot-03

mkdir b:\configdir
cd testroot-03
%PYTHON% ..\test_callbacks.py
endlocal
