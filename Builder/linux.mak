BUILD_TYPE=Release

build: all test kit

all:
	cd ../Source && $(MAKE) -f pysvn_linux_py$(PY_MAJ)$(PY_MIN).mak
clean:
	cd ../Source && $(MAKE) -f pysvn_linux_py$(PY_MAJ)$(PY_MIN).mak clean
	rm -rf ../Kit/Linux/tmp

kit:
	pwd && cd ../Kit/Linux && $(PYTHON) make_rpm.py

install:
	echo sudo may prompt for your password to allow installation of the pysvn rpm
	sudo rpm -i ../Kit/Linux/tmp/RPMS/i386/py${PY_MAJ}${PY_MIN}_pysvn-*-1.i386.rpm

test:
	echo tests are TBD
