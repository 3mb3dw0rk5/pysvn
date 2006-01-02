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
		'SOFTWARE\Python\PythonCore\%(py_maj)d.%(py_min)d\InstallPath',
		'', install_path );
	if not rcb then
	begin
		rcb := RegQueryStringValue( HKCU,
			'SOFTWARE\Python\PythonCore\%(py_maj)d.%(py_min)d\InstallPath',
			'', install_path );
		if not rcb then
		begin
			MsgBox( 'pysvn requires Python %(py_maj)d.%(py_min)d to be installed.' #13 #13
					'Quitting installation',
				 mbError, MB_OK );
		end;
	end;
	Result := rcb;
end;

function pythondir(Default: String): String; 
var
	install_path : string;
	rcb : Boolean;
begin
	rcb := RegQueryStringValue( HKLM,
		'SOFTWARE\Python\PythonCore\%(py_maj)d.%(py_min)d\InstallPath',
		'', install_path );
	if rcb then
	begin
		Result := install_path;
	end
	else
	begin
		rcb := RegQueryStringValue( HKCU,
			'SOFTWARE\Python\PythonCore\%(py_maj)d.%(py_min)d\InstallPath',
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
end;


[Setup]
AppName=Python %(py_maj)d.%(py_min)d PySVN
AppVerName=Python %(py_maj)d.%(py_min)d PySVN %(pysvn_version_string)s 
AppCopyright=Copyright (C) 2003-2006 Barry A. Scott
DefaultDirName={code:pythondir|c:\python%(py_maj)d.%(py_min)d}\lib\site-packages\pysvn
DefaultGroupName=PySVN for Python %(py_maj)d.%(py_min)d
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

Source: "intl3_svn.dll"; DestDir: "{app}"

Source: "libapr.dll"; DestDir: "{app}"
Source: "libaprutil.dll"; DestDir: "{app}"
Source: "libapriconv.dll"; DestDir: "{app}"

Source: "libdb43.dll"; DestDir: "{app}"

Source: "libeay32.dll"; DestDir: "{app}"
Source: "ssleay32.dll"; DestDir: "{app}"

[Icons]
Name: "{group}\PySVN Documentation"; Filename: "{app}\pysvn.html";
Name: "{group}\PySVN License"; Filename: "{app}\pysvn_LICENSE.txt";
Name: "{group}\PySVN Web Site"; Filename: "http://pysvn.tigris.org";
