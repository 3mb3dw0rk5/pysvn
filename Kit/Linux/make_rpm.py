#
#	make_rpm.py
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
pysvn_version_string = '%d.%d.%d' % (pysvn.version[0], pysvn.version[1], pysvn.version[2])
svn_version_string = '%d.%d.%d' % (pysvn.svn_version[0], pysvn.svn_version[1], pysvn.svn_version[2])

build_time  = time.time()
build_time_str = time.strftime( '%d-%b-%Y %H:%M', time.localtime( build_time ) )

tmpdir = os.path.join( os.getcwd(), 'tmp' )
if os.path.exists( tmpdir ):
	print 'Info: Clean up tmp directory'
	os.system( 'rm -rf tmp' )

print 'Info: Create directories'

for kit_dir in [
	tmpdir,
	os.path.join( tmpdir, 'ROOT' ),
	os.path.join( tmpdir, 'BUILD' ),
	os.path.join( tmpdir, 'SPECS' ),
	os.path.join( tmpdir, 'RPMS' ),
	os.path.join( tmpdir, 'ROOT/usr' ),
	os.path.join( tmpdir, 'ROOT/usr/lib' ),
	os.path.join( tmpdir, 'ROOT/usr/lib/python%(pymaj)d.%(pymin)d' % locals() ),
	os.path.join( tmpdir, 'ROOT/usr/lib/python%(pymaj)d.%(pymin)d/site-packages' % locals() ),
	os.path.join( tmpdir, 'ROOT/usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn' % locals() ),
	os.path.join( tmpdir, 'ROOT/usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/Examples' % locals() ),
	os.path.join( tmpdir, 'ROOT/usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/Examples/Client' % locals() ),
	]:
	if not os.path.exists( kit_dir ):
		os.makedirs( kit_dir )


print 'Info: Copy files'
for cp_src, cp_dst_dir_fmt in [
	('../../Source/pysvn/__init__.py',
		'ROOT/usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn'),
	('../../Source/pysvn/_pysvn.so',
		'ROOT/usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn'),
	('%s/libsvncpp.so.0' % os.environ['SVNCPP_LIB'],
		'ROOT/usr/lib'),
	('../../LICENSE.txt',
		'ROOT/usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/LICENSE.txt'),
	('../../Docs/pysvn.html',
		'ROOT/usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn'),
	('../../Docs/pysvn_prog_ref.html',
		'ROOT/usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn'),
	('../../Docs/pysvn_prog_guide.html',
		'ROOT/usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn'),
	('../../Examples/Client/svn_cmd.py',
		'ROOT/usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/Examples/Client'),
	('../../Examples/Client/parse_datetime.py',
		'ROOT/usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/Examples/Client'),
	]:
	print 'Info:  cp %s' % cp_src
	os.system( 'cp -f %s tmp/%s' % (cp_src, cp_dst_dir_fmt % locals()) )

print 'Info: Create tmp/SPECS/pysvn.spec'
f = file('tmp/SPECS/pysvn.spec','w')
f.write('''BuildRoot:	%(tmpdir)s/ROOT
Name:		py%(pymaj)d%(pymin)d_pysvn
Version:	%(pysvnmaj)d.%(pysvnmin)d.%(pysvnpat)d
Group:		Development/Libraries
Release:	1
Summary:	pysvn %(pysvn_version_string)s Python extension for Subversion %(svn_version_string)s
Copyright:	Barry A. Scott (c) 2003-2004
Packager:	Barry A. Scott <barry@barrys-emacs.org>
%%description
PySVN %(pysvn_version_string)s for Python %(python_version_string)s and Subversion %(svn_version_string)s

Copyright Barry A. Scott (c) 2003-2004

mailto:barry@barrys-emacs.org
http://pysvn.tigris.org

     Barry Scott

%%prep
%%build
%%install
%%post
/usr/bin/python%(pymaj)d.%(pymin)d -c "import pysvn"
/usr/bin/python%(pymaj)d.%(pymin)d -O -c "import pysvn"
%%postun
rm -f /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/__init__.pyc
rm -f /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/__init__.pyo
%%files
%%defattr (-,root,root)
%%attr(444,root,root) /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/__init__.py
%%attr(555,root,root) /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/_pysvn.so
%%attr(555,root,root) /usr/lib/libsvncpp.so.0
%%attr(444,root,root) /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/LICENSE.txt
%%attr(444,root,root) /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/pysvn.html
%%attr(444,root,root) /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/pysvn_prog_ref.html
%%attr(444,root,root) /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/pysvn_prog_guide.html
%%attr(555,root,root) /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/Examples/Client/svn_cmd.py
%%attr(444,root,root) /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/Examples/Client/parse_datetime.py

''' % locals() )
f.close()

print 'Info: Create rpmrc'
os.system('grep ^macrofiles: /usr/lib/rpm/rpmrc |sed -e s!~/.rpmmacros!%(tmpdir)s/rpmmacros! >%(tmpdir)s/rpmrc' % locals() )
print 'Info: Create rpmmacros'
f = file( 'tmp/rpmmacros', 'w' )
f.write( '%%_topdir %(tmpdir)s' % locals() )
f.close()
print 'Info: rpmbuild'
os.system( 'rpmbuild --rcfile=/usr/lib/rpm/rpmrc:%(tmpdir)s/rpmrc -bb %(tmpdir)s/SPECS/pysvn.spec' % locals() )
