BUILD_TYPE=Release
BUILD_VERSION=1.4

build: all test kit

all: all-$(BUILD_VERSION)

all-1.4:
	cd ..\Source && devenv pysvn-for-svn-1-4.sln /useenv /build "$(BUILD_TYPE)"  /project "pysvn"

all-1.5:
	cd ..\Source && devenv pysvn-for-svn-1-5.sln /useenv /build "$(BUILD_TYPE)"  /project "pysvn"

clean: clean-$(BUILD_VERSION)

clean-1.4:
	cd ..\Source && devenv pysvn-for-svn-1-4.sln /useenv /clean "$(BUILD_TYPE)"  /project "pysvn"
	cd ..\Source && del sept
	cd ..\Tests && $(MAKE) -f win32.mak clean
	cd ..\kit\Win32 && $(MAKE) clean

clean-1.5:
	cd ..\Source && devenv pysvn-for-svn-1-5.sln /useenv /clean "$(BUILD_TYPE)"  /project "pysvn"
	cd ..\Source && del sept
	cd ..\Tests && $(MAKE) -f win32.mak clean
	cd ..\kit\Win32 && $(MAKE) clean

kit: kit-$(BUILD_VERSION)

kit-1.4:
	cd ..\kit\Win32 && $(MAKE) all_msvc71

kit-1.5:
	cd ..\kit\Win32 && $(MAKE) all_msvc71

install: install-$(BUILD_VERSION)

install-1.4:
	..\kit\Win32\tmp\output\setup.exe

install-1.5:
	..\kit\Win32\tmp\output\setup.exe

test: test-$(BUILD_VERSION)

test-1.4:
	cd  ..\Tests && $(MAKE) -f win32.mak

test-1.5:
	cd  ..\Tests && $(MAKE) -f win32.mak
