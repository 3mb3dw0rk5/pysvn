BUILD_TYPE=Release

build: all test kit

all:
	cd ../Source && $(MAKE) -f pysvn_linux_py$(PY_MAJ)$(PY_MIN)
clean:
	cd ../Source && $(MAKE) -f pysvn_linux_py$(PY_MAJ)$(PY_MIN) clean
	rm -rf ../kit/Linux/tmp

kit:
	cd ../kit/Linux && $(PYTHON) make_rpm.py

install:
	echo sudo may prompt for your password to allow installation of the pysvn rpm
	sudo rpm -i ../kit/Win32/tmp/output/setup.exe

test:
	echo tests are TBD
