BUILD_TYPE=Release

build: all test kit

all:
	cd ../Source && $(MAKE) -f pysvn_freebsd_py$(PY_MAJ)$(PY_MIN).mak

clean:
	cd ../Source && $(MAKE) -f pysvn_freebsd_py$(PY_MAJ)$(PY_MIN).mak clean
	cd ../Tests && $(MAKE) -f unix.mak clean
	rm -rf ../Kit/Linux/tmp

kit:
	pwd && cd ../Kit/FreeBSD && $(PYTHON) make_pkg.py

test:
	cd ../Tests && $(MAKE) -f unix.mak all
