;
; -- bemacs.iss --
;

[Code]
function InitializeSetup(): Boolean;
var
	install_path : string;
	rcb : Boolean;
begin
	rcb := RegQueryStringValue( HKLM,
		'SOFTWARE\Python\PythonCore\2.3\InstallPath',
		'', install_path );
	if not rcb then
	begin
		MsgBox( 'pysvn requires Python 2.3 to be installed.' #13 #13
				'Quitting installation',
			 mbError, MB_OK );
	end;
	Result := rcb;
end;

function pythondir(Default: String): String; 
var
	install_path : string;
	rcb : Boolean;
begin
	rcb := RegQueryStringValue( HKLM,
		'SOFTWARE\Python\PythonCore\2.3\InstallPath',
		'', install_path );
	if rcb then
	begin
		Result := install_path;
	end
	else
	begin
		Result := Default
	end;
end;


[Setup]
AppName=PySVN
AppVerName=PySVN 0.5.0
AppCopyright=Copyright (C) 2003 Barry A. Scott
DefaultDirName={code:pythondir|c:\python23}\lib\site-packages
DefaultGroupName=PySVN
DisableStartupPrompt=yes
Compression=bzip/9

[Files]

#include "msvc60_system_files.iss"

Source: "..\Source\pyds\pysvn.pyd"; DestDir: "{app}";
Source: "..\Docs\pysvn.html"; DestDir: "{app}";
Source: "tmp\pysvn_LICENSE.txt"; DestDir: "{app}";

Source: "tmp\libdb42.dll"; DestDir: "{app}"

Source: "tmp\libeay32.dll"; DestDir: "{app}"
Source: "tmp\ssleay32.dll"; DestDir: "{app}"

[Icons]
Name: "{group}\Documentation"; Filename: "{app}\pysvn.html";
Name: "{group}\License"; Filename: "{app}\pysvn_LICENSE.txt";
Name: "{group}\Web Site"; Filename: "http://pysvn.tigris.org";
