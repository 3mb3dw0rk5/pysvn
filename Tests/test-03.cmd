@prompt $P$S$G
@echo WorkDir: %WORKDIR%
@echo PYTHON: %PY%\python

setlocal
mkdir testroot-03
subst b: %CD%\testroot-03

mkdir b:\configdir
%PY%\python test_callbacks.py
endlocal
