BUILD_TYPE=Release

build: all test kit

all:
	cd ..\Source && msdev pysvn.dsw /make "pysvn - Win32 $(BUILD_TYPE)"

clean:
	cd ..\Source && msdev pysvn.dsw /make "pysvn - Win32 $(BUILD_TYPE)" /clean
	cd ..\Tests && $(MAKE) -f win32.mak clean
	cd ..\kit\Win32 && $(MAKE) clean

kit:
	cd ..\kit\Win32 && $(MAKE) all_msvc60

install:
	..\kit\Win32\tmp\output\setup.exe

test:
	cd  ..\Tests && $(MAKE) -f win32.mak
