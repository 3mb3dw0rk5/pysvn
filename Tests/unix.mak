all: test-01.unix.new.log test-03.unix.new.log

clean:
	-rm -f test-01.unix.new.log
	-rm -f test-03.unix.new.log
	-rm -rf testroot-01
	-rm -rf testroot-03

test-01.unix.new.log: test-01.sh test-01.unix.known_good.log
	-rm -rf testroot-01
	./test-01.sh >test-01.unix.new.log 2>&1
	python benchmark_diff.py test-01.unix.known_good.log test-01.unix.new.log

test-03.unix.new.log: test-03.sh test-03.unix.known_good.log
	-rm -rf testroot-03
	test "$$(id -u -n)" = "barry" && ./test-03.sh >test-03.unix.new.log 2>&1
	test "$$(id -u -n)" = "barry" && python benchmark_diff.py test-03.unix.known_good.log test-03.unix.new.log