all:
	mkdir tmp
	copy ..\LICENSE.txt tmp\pysvn_LICENSE.txt
	copy $(SUBVERSION)\db4-win32\bin\libdb42.dll tmp
	copy $(OPENSSL)\out32dll\libeay32.dll tmp
	copy $(OPENSSL)\out32dll\ssleay32.dll tmp
	"c:\Program Files\Inno Setup 4\ISCC.exe" pysvn.iss

debug:
	"c:\Program Files\Inno Setup 4\Compil32.exe" pysvn.iss

clean:
	-rmdir /s /q tmp
	-rmdir /s/ q Output
