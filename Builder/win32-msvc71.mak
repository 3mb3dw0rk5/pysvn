BUILD_TYPE=Release

build: all test kit

all:
	cd ..\Source && devenv pysvn.sln /useenv /build "$(BUILD_TYPE)"  /project "pysvn"

clean:
	cd ..\Source && devenv pysvn.sln /useenv /clean "$(BUILD_TYPE)"  /project "pysvn"
	cd ..\Tests && $(MAKE) -f win32.mak clean
	cd ..\kit\Win32 && $(MAKE) clean

kit:
	cd ..\kit\Win32 && $(MAKE) all_msvc71

install:
	..\kit\Win32\tmp\output\setup.exe

test:
	cd  ..\Tests && $(MAKE) -f win32.mak
