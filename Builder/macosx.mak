BUILD_TYPE=Release

build: all test kit

all:
	cd ../Source && $(MAKE) -f pysvn_macosx_py$(PY_MAJ)$(PY_MIN).mak
clean:
	cd ../Source && $(MAKE) -f pysvn_macosx_py$(PY_MAJ)$(PY_MIN).mak clean
	cd ../Tests && $(MAKE) -f macosx.mak clean
	rm -rf ../Kit/MacOSX/tmp

kit:
	pwd && cd ../Kit/MacOSX && $(PYTHON) make_pkg.py

test:
	cd ../Tests && $(MAKE) -f macosx.mak all
