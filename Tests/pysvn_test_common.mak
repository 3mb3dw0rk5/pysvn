#
#	pysvn_test_common
#
#	include this mak file after defining the variables it needs
#
all: test-01.unix.new.log test-03.unix.new.log test-04.unix.new.log test-05.unix.new.log test-06.unix.new.log

help:
	@echo "make clean         - clean all tests"
	@echo "make all           - run all tests"
	@echo "make clean-01      - clean test 01"
	@echo "make diff-01       - run wb-diff on the outcome of test 01"
	@echo "make new-01        - use the new log as the known-good log of test 01"

clean: clean-01 clean-03 clean-04 clean-05 clean-06

#
#	Helpers while developing and verifying tests
#
test-01.unix.new.log: test-01.sh test-01.unix.known_good.log
	-rm -rf testroot-01
	$(EXTRA_LIB_VAR_NAME)=$(EXTRA_LIB) PYTHON=$(PYTHON) ./test-01.sh >test-01.unix.new.log 2>&1
	$(PYTHON) benchmark_diff.py test-01.unix.known_good.log test-01.unix.new.log

clean-01:
	-rm -f test-01.unix.new.log
	-rm -f test-01.unix.new.log.clean
	-rm -rf testroot-01

diff-01: test-01.unix.new.log
	wb-diff test-01.unix.known_good.log.clean test-01.unix.new.log.clean

new-01: test-01.unix.new.log
	cp  test-01.unix.new.log test-01.unix.known_good.log

test-03.unix.new.log: test-03.sh test-03.unix.known_good.log
	-rm -rf testroot-03
	if [ "$$(id -u -n)" = "barry" ]; then $(EXTRA_LIB_VAR_NAME)=$(EXTRA_LIB) PYTHON=$(PYTHON) ./test-03.sh >test-03.unix.new.log 2>&1;fi
	if [ "$$(id -u -n)" = "barry" ]; then $(PYTHON) benchmark_diff.py test-03.unix.known_good.log test-03.unix.new.log; fi

clean-03:
	-rm -f test-03.unix.new.log
	-rm -f test-03.unix.new.log.clean
	-rm -rf testroot-03

diff-03: test-03.unix.new.log
	wb-diff test-03.unix.known_good.log.clean test-03.unix.new.log.clean

new-03: test-03.unix.new.log
	cp  test-03.unix.new.log test-03.unix.known_good.log

test-04.unix.new.log: test-04.sh test-04.unix.known_good.log
	-rm -rf testroot-04
	$(EXTRA_LIB_VAR_NAME)=$(EXTRA_LIB) PYTHON=$(PYTHON) ./test-04.sh >test-04.unix.new.log 2>&1
	$(PYTHON) benchmark_diff.py test-04.unix.known_good.log test-04.unix.new.log

clean-04:
	-rm -f test-04.unix.new.log
	-rm -f test-04.unix.new.log.clean
	-rm -rf testroot-04

diff-04: test-04.unix.new.log
	wb-diff test-04.unix.known_good.log.clean test-04.unix.new.log.clean

new-04: test-04.unix.new.log
	cp  test-04.unix.new.log test-04.unix.known_good.log

test-05.unix.new.log: test-05.sh test-05.unix.known_good.log
	-rm -rf testroot-05
	if PYTHONPATH=../Source $(PYTHON) svn_min_version.py 1 2 1; then $(EXTRA_LIB_VAR_NAME)=$(EXTRA_LIB) PYTHON=$(PYTHON) ./test-05.sh >test-05.unix.new.log 2>&1; fi
	if PYTHONPATH=../Source $(PYTHON) svn_min_version.py 1 2 1; then $(PYTHON) benchmark_diff.py test-05.unix.known_good.log test-05.unix.new.log; fi

clean-05:
	-rm -f test-05.unix.new.log
	-rm -f test-05.unix.new.log.clean
	-rm -rf testroot-05

diff-05: test-05.unix.new.log
	wb-diff test-05.unix.known_good.log.clean test-05.unix.new.log.clean

new-05: test-05.unix.new.log
	cp  test-05.unix.new.log test-05.unix.known_good.log

test-06.unix.new.log: test-06.sh test-06.unix.known_good.log
	-rm -rf testroot-06
	if PYTHONPATH=../Source $(PYTHON) svn_min_version.py 1 3 0; then $(EXTRA_LIB_VAR_NAME)=$(EXTRA_LIB) PYTHON=$(PYTHON) ./test-06.sh >test-06.unix.new.log 2>&1; fi
	if PYTHONPATH=../Source $(PYTHON) svn_min_version.py 1 3 0; then $(PYTHON) benchmark_diff.py test-06.unix.known_good.log test-06.unix.new.log; fi

clean-06:
	-rm -f test-06.unix.new.log
	-rm -f test-06.unix.new.log.clean
	-rm -rf testroot-06

diff-06: test-06.unix.new.log
	wb-diff test-06.unix.known_good.log.clean test-06.unix.new.log.clean

new-06: test-06.unix.new.log
	cp  test-06.unix.new.log test-06.unix.known_good.log

