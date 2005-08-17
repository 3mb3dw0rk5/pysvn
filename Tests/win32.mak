all: test-01.win32.new.log test-03.win32.new.log test-04.win32.new.log test-05.win32.new.log

clean: clean-01 clean-03 clean-04 clean-05

clean-01:
	-subst b: /d >nul 2>&1
	if exist test-01.win32.new.log del test-01.win32.new.log
	if exist testroot-01 rmdir /s /q testroot-01

clean-03:
	-subst b: /d >nul 2>&1
	if exist test-03.win32.new.log del test-03.win32.new.log
	if exist testroot-03 rmdir /s /q testroot-03

clean-04:
	-subst b: /d >nul 2>&1
	if exist test-04.win32.new.log del test-04.win32.new.log
	if exist testroot-04 rmdir /s /q testroot-04

clean-05:
	-subst b: /d >nul 2>&1
	if exist test-05.win32.new.log del test-05.win32.new.log
	if exist testroot-05 rmdir /s /q testroot-05

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

test-05.win32.new.log: test-05.cmd test-05.win32.known_good.log
	-subst b: /d >nul 2>&1
	if exist testroot-05 rmdir /s /q testroot-05
	test-05.cmd >test-05.win32.new.log 2>&1
	python benchmark_diff.py test-05.win32.known_good.log test-05.win32.new.log

#
#	Helpers while developing and verifying tests
#
WB_DIFF=wb-diff.cmd
diff-01: test-01.win32.new.log
	$(WB_DIFF) test-01.win32.known_good.log.clean test-01.win32.new.log.clean

new-01: test-01.win32.new.log
	copy test-01.win32.new.log test-01.win32.known_good.log

diff-03: test-03.win32.new.log
	$(WB_DIFF) test-03.win32.known_good.log.clean test-03.win32.new.log.clean

new-03: test-03.win32.new.log
	copy test-03.win32.new.log test-03.win32.known_good.log

diff-04: test-04.win32.new.log
	$(WB_DIFF) test-04.win32.known_good.log.clean test-04.win32.new.log.clean

new-04: test-04.win32.new.log
	copy test-04.win32.new.log test-04.win32.known_good.log

diff-05: test-05.win32.new.log
	$(WB_DIFF) test-05.win32.known_good.log.clean test-05.win32.new.log.clean

new-05: test-05.win32.new.log
	copy test-05.win32.new.log test-05.win32.known_good.log

