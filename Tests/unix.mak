all: test-01.unix.new.log test-03.unix.new.log test-04.unix.new.log test-05.unix.new.log

help:
	@echo "make clean         - clean all tests"
	@echo "make all           - run all tests"
	@echo "make clean-01      - clean test 01"
	@echo "make diff-01       - run wb-diff on the outcome of test 01"
	@echo "make new-01        - use the new log as the known-good log"

clean: clean-01 clean-03 clean-04 clean-05

clean-01:
	-rm -f test-01.unix.new.log
	-rm -f test-01.unix.new.log.clean
	-rm -rf testroot-01

clean-03:
	-rm -f test-03.unix.new.log
	-rm -f test-03.unix.new.log.clean
	-rm -rf testroot-03

clean-04:
	-rm -f test-04.unix.new.log
	-rm -f test-04.unix.new.log.clean
	-rm -rf testroot-04

clean-05:
	-rm -f test-05.unix.new.log
	-rm -f test-05.unix.new.log.clean
	-rm -rf testroot-05

test-01.unix.new.log: test-01.sh test-01.unix.known_good.log
	-rm -rf testroot-01
	./test-01.sh >test-01.unix.new.log 2>&1
	python benchmark_diff.py test-01.unix.known_good.log test-01.unix.new.log

test-03.unix.new.log: test-03.sh test-03.unix.known_good.log
	-rm -rf testroot-03
	if [ "$$(id -u -n)" = "barry" ]; then ./test-03.sh >test-03.unix.new.log 2>&1;fi
	if [ "$$(id -u -n)" = "barry" ]; then python benchmark_diff.py test-03.unix.known_good.log test-03.unix.new.log; fi

test-04.unix.new.log: test-04.sh test-04.unix.known_good.log
	-rm -rf testroot-04
	./test-04.sh >test-04.unix.new.log 2>&1
	python benchmark_diff.py test-04.unix.known_good.log test-04.unix.new.log

test-05.unix.new.log: test-05.sh test-05.unix.known_good.log
	-rm -rf testroot-05
	if PYTHONPATH=../Source ${PYTHON} svn_min_version.py 1 2 1; then ./test-05.sh >test-05.unix.new.log 2>&1; fi
	if PYTHONPATH=../Source ${PYTHON} svn_min_version.py 1 2 1; then python benchmark_diff.py test-05.unix.known_good.log test-05.unix.new.log; fi

#
#	Helpers while developing and verifying tests
#
diff-01: test-01.unix.new.log
	wb-diff test-01.unix.known_good.log.clean test-01.unix.new.log.clean

new-01: test-01.unix.new.log
	cp  test-01.unix.new.log test-01.unix.known_good.log

diff-03: test-03.unix.new.log
	wb-diff test-03.unix.known_good.log.clean test-03.unix.new.log.clean

new-03: test-03.unix.new.log
	cp  test-03.unix.new.log test-03.unix.known_good.log

diff-04: test-04.unix.new.log
	wb-diff test-04.unix.known_good.log.clean test-04.unix.new.log.clean

new-04: test-04.unix.new.log
	cp  test-04.unix.new.log test-04.unix.known_good.log

diff-05: test-05.unix.new.log
	wb-diff test-05.unix.known_good.log.clean test-05.unix.new.log.clean

new-05: test-05.unix.new.log
	cp  test-05.unix.new.log test-05.unix.known_good.log

