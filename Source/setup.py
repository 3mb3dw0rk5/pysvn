#
# ====================================================================
# (c) 2005 Barry A Scott.  All rights reserved.
#
# This software is licensed as described in the file LICENSE.txt,
# which you should have received as part of this distribution.
#
# ====================================================================
#
#
#   setup.py
#
#   make it easy to build pysvn outside of svn
#
import sys
import os
import distutils
import distutils.sysconfig

class SetupError(Exception):
    pass

is_darwin = sys.platform == 'darwin'

def main( argv ):
    if sys.platform == 'win32':
        print 'Error: Works for Unix like systems only'
        return 1

    try:
        if argv[1:2] == ['configure']:
            return create_makefile( argv )
        elif argv[1:2] == ['help']:
            return help( argv )
        else:
            return help( argv )
    except SetupError, e:
        print 'Error:',str(e)
        return 1

def help( argv ):
    basename = os.path.basename( argv[0] )
    print '''

    Create a makefile for this python and platform
        python %s configure <options>

    Help
        python %s help

    where <options> is one of more of:
        --pycxx-dir=<dir>
        --apr-inc-dir=<dir>
        --svn-inc-dir=<dir>
        --svn-lib-dir=<dir>

''' % (basename, basename)
    return 1

def create_makefile( argv ):

    include_dir_list = []

    # add python include dirs
    include_dir_list.append( distutils.sysconfig.get_python_inc() )
    if distutils.sysconfig.get_python_inc() != distutils.sysconfig.get_python_inc( True ):
        include_dir_list.append( distutils.sysconfig.get_python_inc( True ) )

    # add pycxx include
    pycxx_dir = find_pycxx( argv )
    include_dir_list.append( pycxx_dir )
    # add SVN include
    include_dir_list.append( find_svn_inc( argv ) )
    # add APR include
    include_dir_list.append( find_apr_inc( argv ) )

    # add source dir
    include_dir_list.append( '.' )

    template_values = {
        # python executable
        'python_exe':       sys.executable,

        # includes
        'includes':         ' '.join( ['-I%s' % include_dir for include_dir in include_dir_list] ),

        # add svn lib dir
        'svn_lib_dir':      find_svn_lib( argv ),

        # pycxx src dir
        'pycxx_dir':        pycxx_dir
        }

    print 'Info: Creating Makefile'
    makefile = file( 'Makefile', 'w' )
    if sys.platform == 'darwin':
        makefile.write( makefile_template_macosx % template_values )
    else:
        makefile.write( makefile_template % template_values )
    makefile.close()

makefile_template = '''#
#	Created by pysvn Extension/Source/setup.py
#
PYTHON=%(python_exe)s
CCC=g++ -c
CCCFLAGS=-fPIC -fexceptions -frtti %(includes)s
PYCXX=%(pycxx_dir)s
LDSHARED=g++ -shared
LDLIBS=-L%(svn_lib_dir)s \
-lsvn_client-1 \
-lsvn_repos-1

include pysvn_common.mak
'''


makefile_template_macosx = '''#
#	Created by pysvn Extension/Source/setup.py
#
PYTHON=%(python_exe)s
CCC=g++ -c
CCCFLAGS=-Wno-long-double -fPIC -fexceptions -frtti %(includes)s
PYCXX=%(pycxx_dir)s
LDSHARED=g++ -bundle -g -u _PyMac_Error -framework System -framework Python
LDLIBS= \
%(svn_lib_dir)s/libsvn_client-1.a \
%(svn_lib_dir)s/libsvn_subr-1.a \
%(svn_lib_dir)s/libsvn_wc-1.a \
%(svn_lib_dir)s/libsvn_ra-1.a \
%(svn_lib_dir)s/libsvn_ra_dav-1.a \
%(svn_lib_dir)s/libsvn_ra_local-1.a \
%(svn_lib_dir)s/libsvn_ra_svn-1.a \
%(svn_lib_dir)s/libsvn_delta-1.a \
%(svn_lib_dir)s/libsvn_repos-1.a \
%(svn_lib_dir)s/libsvn_fs-1.a \
%(svn_lib_dir)s/libsvn_fs_fs-1.a \
%(svn_lib_dir)s/libsvn_fs_base-1.a \
%(svn_lib_dir)s/libsvn_diff-1.a \
%(svn_lib_dir)s/libaprutil-0.a \
%(svn_lib_dir)s/libapr-0.a \
%(svn_lib_dir)s/libneon.a \
%(svn_lib_dir)s/libssl.a \
%(svn_lib_dir)s/libcrypto.a \
%(svn_lib_dir)s/libexpat.a \
%(svn_lib_dir)s/libxml2.a \
%(svn_lib_dir)s/libdb-4.2.a \
%(svn_lib_dir)s/libintl.a \
\
 -liconv -lz
include pysvn_common.mak
'''

def find_pycxx( argv ):
    return find_dir( argv,
                'PyCXX include',
                '--pycxx-dir=',
                [   '../Import/pycxx_5_3_3'],
                'CXX/Version.hxx' )

def find_svn_inc( argv ):
    return find_dir( argv,
                'SVN include',
                '--svn-inc-dir=',
                [   '/usr/include/subversion-1',            # typical Linux
                    '/usr/local/include/subversion-1',      # typical *BSD
                    '/sw/include/subversion-1'],            # Darwin
                'svn_client.h' )

def find_svn_lib( argv ):
    return find_dir( argv,
                'SVN library',
                '--svn-lib-dir=',
                [   '/usr/lib',                             # typical Linux
                    '/usr/local/lib',                       # typical *BSD
                    '/sw/lib'],                             # Darwin
                is_darwin and 'libsvn_client-1.dylib' or 'libsvn_client-1.so' )

def find_apr_inc( argv ):
    return find_dir( argv,
                'APR include',
                '--apr-inc-dir=',
                [   '/usr/include/apr-0',                   # typical Linux
                    '/usr/include/apache2',                 # alternate Linux
                    '/usr/local/include/apr0',              # typical *BSD
                    '/usr/local/include/apache2',           # alternate *BSD
                    '/sw/include/apr-0'],                   # Darwin
                'apr.h' )

def find_apr_lib( argv ):
    return find_dir( argv,
                'APR library',
                '--apr-lib-dir=',
                [   '/usr/lib',                             # typical Linux
                    '/usr/local/lib',                       # typical *BSD
                    '/sw/lib'],                             # Darwin
                is_darwin and 'libapr-0.dylib' or 'libapr-0.so' )

def find_dir( argv, name, kw, base_dir_list, check_file=None ):
    dir = __find_dir( argv, name, kw, base_dir_list, check_file )
    print 'Info: Found',name,'in',dir
    return dir

def __find_dir( argv, name, kw, base_dir_list, check_file=None ):
    for arg in argv[2:]:
        if arg[0:len(kw)] == kw:
            return arg[len(kw):]

    # expect to find the pycxx in this source tree
    for dir in base_dir_list:
        print 'Info: Looking for',name,'in',dir
        if check_file is not None:
            full_check_file = os.path.join( dir, check_file )
            print 'Info: Checking for',full_check_file
            if os.path.exists( full_check_file ):
                return os.path.abspath( dir )
        elif os.path.exists( dir ):
            return os.path.abspath( dir )

    raise SetupError, 'cannot find %s - use %s' % (name, kw)


if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
