all: kit

kit: pysvn-branded.iss info_before.txt copy_setup.cmd
	copy ..\LICENSE.txt tmp\pysvn_LICENSE.txt
	copy $(SUBVERSION)\db4-win32\bin\libdb42.dll tmp
	copy $(OPENSSL)\out32dll\libeay32.dll tmp
	copy $(OPENSSL)\out32dll\ssleay32.dll tmp
	"c:\Program Files\Inno Setup 4\ISCC.exe" tmp\pysvn-branded.iss
	tmp\setup_copy.cmd

info_before.txt: pysvn-branded.iss

copy_setup.cmd: pysvn-branded.iss

pysvn-branded.iss: setup_version_handling.py pysvn.iss
	if not exist tmp mkdir tmp
	python setup_version_handling.py

debug:
	"c:\Program Files\Inno Setup 4\Compil32.exe" pysvn.iss

clean:
	if exist tmp rmdir /s /q tmp
