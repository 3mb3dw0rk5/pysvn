BUILD_TYPE=Release

build: all test kit

all:	../Source/Makefile
	cd ../Source && $(MAKE)

clean:	../Source/Makefile
	cd ../Source && $(MAKE) clean && rm Makefile
	cd ../Tests && $(MAKE) -f unix.mak clean

../Source/Makefile: ../Source/setup.py
	cd ../Source && $(PYTHON) setup.py configure
	

kit:
	cd ../Kit/MacOSX && $(PYTHON) make_pkg.py

test:
	cd ../Tests && $(MAKE) -f unix.mak all
