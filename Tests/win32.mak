all: test-04.win32.new.log test-01.win32.new.log test-03.win32.new.log

clean:
	if exist test-01.win32.new.log del test-01.win32.new.log
	if exist test-03.win32.new.log del test-03.win32.new.log
	if exist test-04.win32.new.log del test-04.win32.new.log
	-subst b: /d >nul 2>&1
	if exist testroot-01 rmdir /s /q testroot-01
	if exist testroot-03 rmdir /s /q testroot-03
	if exist testroot-04 rmdir /s /q testroot-04

test-01.win32.new.log: test-01.cmd test-01.win32.known_good.log
	-subst b: /d >nul 2>&1
	if exist testroot-01 rmdir /s /q testroot-01
	test-01.cmd >test-01.win32.new.log 2>&1
	python benchmark_diff.py test-01.win32.known_good.log test-01.win32.new.log

test-03.win32.new.log: test-03.cmd test-03.win32.known_good.log
	-subst b: /d >nul 2>&1
	if exist testroot-03 rmdir /s /q testroot-03
	if "%USERNAME%" == "barry" test-03.cmd >test-03.win32.new.log 2>&1
	if "%USERNAME%" == "barry" python benchmark_diff.py test-03.win32.known_good.log test-03.win32.new.log

test-04.win32.new.log: test-04.cmd test-04.win32.known_good.log
	-subst b: /d >nul 2>&1
	if exist testroot-04 rmdir /s /q testroot-04
	test-04.cmd >test-04.win32.new.log 2>&1
	python benchmark_diff.py test-04.win32.known_good.log test-04.win32.new.log
