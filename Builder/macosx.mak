BUILD_TYPE=Release

build: all kit

all:
	cd ../Source && $(MAKE) -f pysvn_macosx_py$(PY_MAJ)$(PY_MIN).mak
clean:
	cd ../Source && $(MAKE) -f pysvn_macosx_py$(PY_MAJ)$(PY_MIN).mak clean
	rm -rf ../Kit/MacOSX/tmp

kit:
	pwd && cd ../Kit/MacOSX && $(PYTHON) make_pkg.py
