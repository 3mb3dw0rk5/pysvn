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
AppVerName=PySVN UNCONTROLLED
AppCopyright=Copyright (C) 2003-2004 Barry A. Scott
DefaultDirName={code:pythondir|c:\python23}\lib\site-packages\pysvn
DefaultGroupName=PySVN for Python 2.3
DisableStartupPrompt=yes
InfoBeforeFile=info_before.txt
Compression=bzip/9

[Files]

#include "..\msvc60_system_files.iss"

Source: "..\..\..\Source\pysvn\__init__.py"; DestDir: "{app}";
Source: "..\..\..\Source\pysvn\_pysvn.pyd"; DestDir: "{app}";
Source: "..\..\..\Docs\pysvn.html"; DestDir: "{app}";
Source: "..\..\..\Docs\pysvn_prog_guide.html"; DestDir: "{app}";
Source: "..\..\..\Docs\pysvn_prog_ref.html"; DestDir: "{app}";
Source: "LICENSE.txt"; DestDir: "{app}";

Source: "..\..\..\Examples\Client\svn_cmd.py"; DestDir: "{app}\Examples\Client";
Source: "..\..\..\Examples\Client\parse_datetime.py"; DestDir: "{app}\Examples\Client";

Source: "libdb42.dll"; DestDir: "{app}"

Source: "libeay32.dll"; DestDir: "{app}"
Source: "ssleay32.dll"; DestDir: "{app}"

[Icons]
Name: "{group}\Documentation"; Filename: "{app}\pysvn.html";
Name: "{group}\License"; Filename: "{app}\pysvn_LICENSE.txt";
Name: "{group}\Web Site"; Filename: "http://pysvn.tigris.org";
