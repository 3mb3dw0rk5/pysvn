print 'Info: setup_version_handling.py'
import sys
sys.path.insert( 0, '..\\..\\Source')
import pysvn
import time

python_version_string = '%d.%d.%d' % (sys.version_info[0], sys.version_info[1], sys.version_info[2])
pysvn_version_string = '%d.%d.%d' % (pysvn.version[0], pysvn.version[1], pysvn.version[2])
svn_version_string = '%d.%d.%d-%s' % pysvn.svn_version

build_time  = time.time()
build_time_str = time.strftime( '%d-%b-%Y %H:%M', time.localtime( build_time ) )

print 'Info: Create info_before.txt'

f = file('tmp\\info_before.txt','w')
f.write(
'''PySVN %s for Python %s and Subversion %s

    Barry Scott

    %s

''' % (pysvn_version_string, python_version_string, svn_version_string, build_time_str) )
f.close()

print 'Info: Creating pysvn-branded.iss from pysvn.iss'
f = file( 'pysvn.iss', 'r' )
all_lines = f.readlines()
f.close()
f = file( 'tmp\\pysvn-branded.iss', 'w' )
for line in all_lines:
	if line.find( 'AppVerName=' ) == 0:
		f.write( 'AppVerName=Python %d.%d PySVN %s\n' %
			(sys.version_info[0], sys.version_info[1], pysvn_version_string) )
	else:
		f.write( line )
f.close()

print 'Info: Create setup_copy.cmd'
f = file( 'tmp\\setup_copy.cmd', 'w' )
f.write( 'copy tmp\\Output\\setup.exe tmp\\Output\\pysvn-%s-py%d%d-svn%s.exe\n' %
	(pysvn_version_string, sys.version_info[0], sys.version_info[1], svn_version_string) )
f.close()
