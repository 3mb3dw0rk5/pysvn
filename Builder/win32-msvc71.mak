BUILD_TYPE=Release

build: all test kit

all:
	cd ..\Source && devenv pysvn.sln /build "$(BUILD_TYPE)"  /project "pysvn"

clean:
	cd ..\Source && devenv pysvn.sln /clean "$(BUILD_TYPE)"  /project "pysvn"
	cd ..\kit\Win32 && $(MAKE) clean

kit:
	cd ..\kit\Win32 && $(MAKE) all

install:
	..\kit\Win32\tmp\output\setup.exe

test:
	cd  ..\Tests && test-01.cmd
