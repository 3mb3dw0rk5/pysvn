BUILD_TYPE=Debug

build: all test

all:
	cd ..\Extension && msdev pysvn.dsw /make "pysvn - Win32 $(BUILD_TYPE)"

clean:
	cd ..\Extension && msdev pysvn.dsw /make /clean "pysvn - Win32 $(BUILD_TYPE)"

test:
	cd  ..\Regression && test-01.cmd
