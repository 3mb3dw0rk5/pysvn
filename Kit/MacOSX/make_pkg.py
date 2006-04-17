#
#    make_pkg.py
#
import os

print 'Info: setup_version_handling.py'
import sys
sys.path.insert( 0, '../../Source')
import pysvn
import time

pymaj, pymin, pypat, _, _ = sys.version_info
python_version_string = '%d.%d.%d' % (pymaj, pymin, pypat)
pysvnmaj, pysvnmin, pysvnpat, _ = pysvn.version
pysvn_version_string = '%d.%d.%d-%d' % (pysvn.version[0], pysvn.version[1], pysvn.version[2], pysvn.version[3])
svn_version_package_string = '%d%d%d' % (pysvn.svn_version[0], pysvn.svn_version[1], pysvn.svn_version[2])
svn_version_string = '%d.%d.%d' % (pysvn.svn_version[0], pysvn.svn_version[1], pysvn.svn_version[2])

pkg_filename = 'py%s%s_pysvn_svn%s-%s' % (pymaj, pymin, svn_version_package_string, pysvn_version_string)
print 'Info: Packageing %s' % pkg_filename
build_time  = time.time()
build_time_str = time.strftime( '%d-%b-%Y %H:%M', time.localtime( build_time ) )
year = time.strftime( '%Y', time.localtime( build_time ) )
tmpdir = os.path.join( os.getcwd(), 'tmp' )

if pymaj == 2 and pymin == 3:
    install_dir = '/System/Library/Frameworks/Python.framework/Versions/2.3/lib/python2.3/site-packages'
elif pymaj == 2 and pymin == 4:
    install_dir = '/Library/Frameworks/Python.framework/Versions/2.4/lib/python2.4/site-packages'
else:
    raise RuntimeError( 'Unsupported version of python' )

if os.path.exists( tmpdir ):
    print 'Info: Clean up tmp directory'
    os.system( 'rm -rf tmp' )

print 'Info: Create directories'

for kit_dir in [
    tmpdir,
    os.path.join( tmpdir, 'Resources' ),
    os.path.join( tmpdir, 'Contents' ),
    os.path.join( tmpdir, 'Contents/pysvn' ),
    os.path.join( tmpdir, '%s' % pkg_filename),
    os.path.join( tmpdir, '%s/Examples' % pkg_filename ),
    os.path.join( tmpdir, '%s/Examples/Client' % pkg_filename ),
    os.path.join( tmpdir, '%s/Documentation' % pkg_filename),
    ]:
    if not os.path.exists( kit_dir ):
        os.makedirs( kit_dir )


print 'Info: Finding dylibs used by pysvn'
dylib_list = []
os.system( 'otool -L ../../Source/pysvn/_pysvn.so >/tmp/pysvn_otool.tmp' )
for line in file( '/tmp/pysvn_otool.tmp' ).readlines():
    line = line.strip()
    if( line.startswith( '/' )
    and not line.startswith( '/usr' )
    and not line.startswith( '/System' ) ):
        libpath = line.split()[0]
        print 'Info: Need lib',libpath
        dylib_list.append( libpath )
        
print 'Info: Copy files'

cp_list = [
    ('../../Source/pysvn/__init__.py',
        'Contents/pysvn'),
    ('../../Source/pysvn/_pysvn.so',
        'Contents/pysvn'),
    ('../../LICENSE.txt',
        'Resources/License.txt'),
    ('../../LICENSE.txt',
        '%s/License.txt' % pkg_filename ),
    ('../../Docs/pysvn.html',
        '%s/Documentation' % pkg_filename ),
    ('../../Docs/pysvn_prog_ref.html',
        '%s/Documentation' % pkg_filename ),
    ('../../Docs/pysvn_prog_ref.js',
        '%s/Documentation' % pkg_filename ),
    ('../../Docs/pysvn_prog_guide.html',
        '%s/Documentation' % pkg_filename ),
    ('../../Examples/Client/svn_cmd.py',
        '%s/Examples/Client' % pkg_filename ),
    ('../../Examples/Client/parse_datetime.py',
        '%s/Examples/Client' % pkg_filename ),
    ]

for libpath in dylib_list:
    cp_list.append( (libpath, 'Contents/pysvn') )

for cp_src, cp_dst_dir_fmt in cp_list:
    print 'Info:  cp %s' % cp_src
    os.system( 'cp -f %s tmp/%s' % (cp_src, cp_dst_dir_fmt % locals()) )

print 'Info: Fix the install paths for the dylib files'
for libpath in dylib_list:
    os.system( 'install_name_tool'
        ' -change'
        ' %s'
        ' %s/pysvn/%s'
        ' %s' %
            (libpath, install_dir, os.path.basename( libpath ), 'tmp/Contents/pysvn/_pysvn.so') )

print 'Info: Create tmp/Resources/ReadMe.txt'
f = file('tmp/Resources/ReadMe.txt','w')
f.write('''<html>
<body>
<h1>PySVN %(pysvn_version_string)s for Apple Mac OS X Python %(pymaj)s.%(pymin)s and Subversion %(svn_version_string)s</h1>

<h2>Copyright Barry A. Scott (c) 2003-%(year)s</h2>

<h2>Mail <a href="mailto:barry@barrys-emacs.org">barry@barrys-emacs.org</a></h2>

<h2>Pysvn home <a href="http://pysvn.tigris.org">http://pysvn.tigris.org</a></h2>

<h2>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Barry Scott</h2>
</body>
</html>
''' % locals() )
f.close()

print 'Info: Create tmp/Info.plist'
f = file('tmp/Info.plist','w')
f.write('''<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleGetInfoString</key>
    <string>pysvn Extension %(pysvn_version_string)s</string>
    <key>CFBundleIdentifier</key>
    <string>org.tigris.pysvn.extension</string>
    <key>CFBundleName</key>
    <string>pysvn Extension</string>
    <key>CFBundleShortVersionString</key>
    <string>%(pysvn_version_string)s</string>
    <key>IFMajorVersion</key>
    <integer>%(pysvnmaj)s</integer>
    <key>IFMinorVersion</key>
    <integer>%(pysvnmin)s</integer>
    <key>IFPkgFlagAllowBackRev</key>
    <false/>
    <key>IFPkgFlagAuthorizationAction</key>
    <string>AdminAuthorization</string>
    <key>IFPkgFlagDefaultLocation</key>
    <string>%(install_dir)s</string>
    <key>IFPkgFlagInstallFat</key>
    <false/>
    <key>IFPkgFlagIsRequired</key>
    <false/>
    <key>IFPkgFlagRelocatable</key>
    <false/>
    <key>IFPkgFlagRestartAction</key>
    <string>NoRestart</string>
    <key>IFPkgFlagRootVolumeOnly</key>
    <true/>
    <key>IFPkgFlagUpdateInstalledLanguages</key>
    <false/>
    <key>IFPkgFormatVersion</key>
    <real>0.10000000149011612</real>
</dict>
</plist>
''' % locals() )
f.close()

print 'Info: Create tmp/Description.plist'
f = file('tmp/Description.plist','w')
f.write('''<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>IFPkgDescriptionDescription</key>
	<string>PySVN Extension
</string>
	<key>IFPkgDescriptionTitle</key>
	<string>PySVN Extension</string>
</dict>
</plist>
''' )
f.close()

print 'Info: PackageMaker'
cmd = [    '/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker',
    '-build',
    '-p %s' % os.path.abspath( 'tmp/%s/%s.pkg' % (pkg_filename, pkg_filename) ),
    '-f %s' % os.path.abspath( 'tmp/Contents' ),
    '-r %s' % os.path.abspath( 'tmp/Resources' ),
    '-i %s' % os.path.abspath( 'tmp/Info.plist' ),
    '-d %s' % os.path.abspath( 'tmp/Description.plist' ),
    ]
os.system( ' '.join( cmd ) )

print 'Info: Make Disk Image'
os.system( 'hdiutil create -srcfolder tmp/%s tmp/tmp.dmg' % pkg_filename )
os.system( 'hdiutil convert tmp/tmp.dmg -format UDZO -imagekey zlib-level=9 ' 
        '-o tmp/%s.dmg' % pkg_filename )
