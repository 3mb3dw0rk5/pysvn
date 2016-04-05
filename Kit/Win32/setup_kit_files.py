print( 'Info: setup_kit_files.py' )
import sys
import time
import os
import datetime

def main( argv ):
    inno = InnoSetup( argv )
    return inno.createInnoInstall

class InnoSetup( argv ):
    def __init__( self, argv

        self.arch = sys.argv[1]
        self.vc_ver = sys.argv[2]

        sys.path.insert( 0, '..\\..\\Source')
        import pysvn

        self.py_maj = sys.version_info[0]
        self.py_min = sys.version_info[1]

        self.python_version_string = '%d.%d.%d' % (self.py_maj, self.py_min, sys.version_info[2])
        self.pysvn_version_string = '%d.%d.%d-%d' % (pysvn.version[0], pysvn.version[1], pysvn.version[2], pysvn.version[3])
        self.svn_version_package_string = '%d%d%d' % (pysvn.svn_version[0], pysvn.svn_version[1], pysvn.svn_version[2])
        self.svn_version_string = '%d.%d.%d' % (pysvn.svn_version[0], pysvn.svn_version[1], pysvn.svn_version[2])

        self.build_time  = time.time()
        self.build_time_str = time.strftime( '%d-%b-%Y %H:%M', time.localtime( self.build_time ) )

        self.year = datetime.datetime.now().year

        self.all_code_items = []
        self.all_setup_items
        self.all_file_items = []
        self.all_run_items = []

    def createInnoInstall( self ):
        print( 'Info: Create info_before.txt' )

        f = open( 'tmp\\info_before.txt', 'w' )
        f.write(
'''PySVN %s for %s Python %s and Subversion %s

    Barry Scott

    %s

''' % (self.pysvn_version_string, self.arch, self.python_version_string, self.svn_version_string, self.build_time_str) )
        f.close()

        print( 'Info: Creating pysvn-branded.iss from pysvn.iss' )
        f = open( 'pysvn.iss', 'r' )
        pysvn_iss_text = f.read()
        f.close()

        print( 'Info: Create setup_copy.cmd' )
        f = open( 'tmp\\setup_copy.cmd', 'w' )
        f.write( 'copy tmp\\Output\\setup.exe tmp\\Output\\py%d%d-pysvn-svn%s-%s-%s.exe\n' %
                    (py_maj, py_min, svn_version_package_string, pysvn_version_string, arch) )
        f.close()

        self.all_setup_items.extend( [
                'AppName=Python %(py_maj)d.%(py_min)d PySVN for %(arch)s' % self,
                'AppVerName=Python %(py_maj)d.%(py_min)d PySVN %(pysvn_version_string)s on %(arch)s' % self,
                'AppCopyright=Copyright (C) 2003-%(year)s Barry A. Scott' % self,
                'DefaultDirName={code:pythondir}\lib\site-packages\pysvn',
                'DefaultGroupName=PySVN for Python %(py_maj)d.%(py_min)d on %(arch)s' % self,
                'DisableStartupPrompt=yes',
                'InfoBeforeFile=info_before.txt',
                'Compression=bzip/9',
                ] )

        self.all_file_items.extend( [
                'Source: "..\..\..\Source\pysvn\__init__.py"; DestDir: "{app}";',
                'Source: "..\..\..\Source\pysvn\_pysvn_%(py_maj)d_%(py_min)d.pyd"; DestDir: "{app}"; Flags: ignoreversion;' % self,
                'Source: "..\..\..\Docs\pysvn.html"; DestDir: "{app}";',
                'Source: "..\..\..\Docs\pysvn_prog_guide.html"; DestDir: "{app}";',
                'Source: "..\..\..\Docs\pysvn_prog_ref.html"; DestDir: "{app}";',
                'Source: "..\..\..\Docs\pysvn_prog_ref.js"; DestDir: "{app}";',
                'Source: "LICENSE.txt"; DestDir: "{app}";',
                '',
                'Source: "..\..\..\Examples\Client\svn_cmd.py"; DestDir: "{app}\Examples\Client";',
                'Source: "..\..\..\Examples\Client\parse_datetime.py"; DestDir: "{app}\Examples\Client";',
                ] )

        for dll in [dll for dll in os.listdir( 'tmp' ) if dll.lower().endswith( '.dll' )]:
            self.all_file_items.append( 'Source: "%s"; DestDir: "{app}"; Flags: ignoreversion' % (dll,) )

        if self.vc_ver = '9.0':
            redist_year = '2008'

        elif self.vc_ver = '14.0':
            redist_year = '2015'

        else:
            print( 'Error: Unsupported VC_VER of %s' % (self.vc_ser,) )
            return 1

        if self.arch == 'Win32':
            redist_arch = 'x86'
            code_file = 'pysvn_win32_code.iss'

        elif self.arch == 'Win64':
            redist_arch = 'x64'
            code_file = 'pysvn_win32_code.iss'

        else:
            print( 'Error: Unsupported ARCH of %s' % (self.arch,) )
            return 1

        f = open( code_file, 'r' )
        all_file_items.append( f.read() )
        f.close()

        all_file_items.append( code )

        redist_file = 'vcredist_%s_%s.exe' % (redist_year, redist_arch)
        self.all_file_items.append( 'Source: "%s"; DestDir: {tmp}; Flags: deleteafterinstall' % (redist_file,) )
        self.all_run_items.append( r'Filename: {tmp}\%s; Parameters: "/q"; StatusMsg: Installing VC++ %s %s Redistributables...' % (redist_file, redist_year, self.arch) )

        f = open( 'tmp\\pysvn-branded.iss', 'w' )
        f.write( '[Code]\n' )
        for item in all_code_items:
            f.write( item )
            f.write( '\n' )

        f.write( '[Setup]\n' )
        for item in all_setup_items:
            f.write( item )
            f.write( '\n' )

        f.write( '[Files]\n' )
        for item in all_file_items:
            f.write( item )
            f.write( '\n' )

        f.write( '[Run]\n' )
        for item in all_run_items:
            f.write( item )
            f.write( '\n' )

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
