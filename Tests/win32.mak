all: test-01.win32.new.log test-03.win32.new.log

clean:
	-del test-01.win32.new.log
	-del test-03.win32.new.log

test-01.win32.new.log: test-01.cmd test-01.win32.known_good.log
	test-01.cmd >test-01.win32.new.log 2>&1
	python benchmark_diff.py test-01.win32.known_good.log test-01.win32.new.log

test-03.win32.new.log: test-03.cmd test-03.win32.known_good.log
	if "%USERNAME%" == "barry" test-03.cmd >test-03.win32.new.log 2>&1
	if "%USERNAME%" == "barry" python benchmark_diff.py test-03.win32.known_good.log test-03.win32.new.log
