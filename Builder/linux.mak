BUILD_TYPE=Release

build: all test kit

all:	../Source/Makefile
	cd ../Source && $(MAKE)

clean:	../Source/Makefile
	cd ../Source && $(MAKE) clean && rm Makefile
	cd ../Tests && $(MAKE) -f unix.mak clean
	rm -rf ../Kit/Linux/tmp

../Source/Makefile: ../Source/setup.py
	cd ../Source && $(PYTHON) setup.py configure
	

kit:
	cd ../Kit/Linux && $(PYTHON) make_rpm.py

install:
	echo sudo may prompt for your password to allow installation of the pysvn rpm
	sudo rpm -i ../Kit/Linux/tmp/RPMS/i386/py${PY_MAJ}${PY_MIN}_pysvn-*-1.i386.rpm

test:
	cd ../Tests && $(MAKE) -f unix.mak all
