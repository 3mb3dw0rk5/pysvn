BUILD_TYPE=Release

build: all test kit

all:
	cd ..\Source && msdev pysvn.dsw /make "pysvn - Win32 $(BUILD_TYPE)"

clean:
	cd ..\Source && msdev pysvn.dsw /make "pysvn - Win32 $(BUILD_TYPE)" /clean
	cd ..\kit && $(MAKE) clean

kit:
	cd ..\kit && $(MAKE) all

install:
	..\kit\tmp\output\setup.exe

test:
	cd  ..\Tests && test-01.cmd
