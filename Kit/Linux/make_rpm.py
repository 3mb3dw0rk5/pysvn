#
#	make_rpm.py
#
import os

print 'Info: setup_version_handling.py'
import sys
sys.path.insert( 0, '../../Source/pyds')
import pysvn
import time

pymaj, pymin, pypat, _ = sys.version_info
python_version_string = '%d.%d.%d' % (pymaj, pymin, pypat)
pysvn_version_string = '%d.%d.%d' % (pysvn.version[0], pysvn.version[1], pysvn.version[2])
svn_version_string = '%d.%d.%d-%s' % pysvn.svn_version

build_time  = time.time()
build_time_str = time.strftime( '%d-%b-%Y %H:%M', time.localtime( build_time ) )

print 'Info: Create directories'

tmpdir = os.path.join( os.getcwd(), 'tmp' )

for kit_dir in [
	tmpdir,
	os.path.join( tmpdir, 'ROOT' ),
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


for cp_src, cp_dst_dir_fmt in [
	('../../Sources/pysvn/__init__.py',
		'ROOT/usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn'),
	('../../Examples/Client/svn_cmd.py',
		'ROOT/usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/Examples/Client'),
	('../../Examples/Client/parse_datetime.py',
		'ROOT/usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/Examples/Client'),
	]:
	os.system( 'cp -f %s %s' % (cp_src, cp_dst_dir_fmt % locals()) )

print 'Info: Create tmp/SPECS/pysvn.spec'
f = file('tmp/pysvn.spec','w')
f.write('''BuildRoot:	%(tmpdir)s/ROOT
Name:		py%(pymaj)d%(pymin)d_pysvn
Version:	%(pysvn_version)s
Release:	%(pysvn_version)s
Summary:	pysvn %(pysvn_version)s Python extension for Subversion %(svn_version_string)s
Copyright:	Barry A. Scott (c) 2003-2004
Packager:	Barry A. Scott <barry@barrys-emacs.org>
%description
PySVN %(pysvn_version)s for Python %(python_version)s and Subversion %(subversion_version)s

Copyright Barry A. Scott (c) 2003-2004

mailto:barry@barrys-emacs.org
http://pysvn.tigris.org

     Barry Scott

%prep
%build
%install
%post
/usr/bin/python%(pymaj)d.%(pymin)d -c "import pysvn"
/usr/bin/python%(pymaj)d.%(pymin)d -O -c "import pysvn"
%postun
rm -f /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/__init__.pyc
rm -f /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/__init__.pyo
%files
%defattr (-,root,root)
%attr(444,root,root) /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/__init__.py
%attr(444,root,root) /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/pysvn.so
%attr(444,root,root) /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/pysvn_LICENSE.txt
%attr(555,root,root) /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/Example/Client/svn_cmd.py
%attr(444,root,root) /usr/lib/python%(pymaj)d.%(pymin)d/site-packages/pysvn/Example/Client/parse_datetime.py

''' % locals() )
f.close()


os.system('grep ^macrofiles: /usr/lib/rpm/rpmrc |sed -e s!~/.rpmmacros!%(tmpdir)s/rpmmacros! >%(tmpdir)s/rpmrc' % locals() )
f = file( 'tmp/rpmmacros', 'w' )
f.write( '%%_topdir %(tmpdir)s' % locals() )
f.close()
os.system( 'rpmbuild --rcfile=/usr/lib/rpm/rpmrc:%(tmpdir)s/rpmrc -bb %(tmpdir)s/SPECS/bemacs-with-build-root.spec' )
