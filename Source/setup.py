#
# ====================================================================
# (c) 2005-2006 Barry A Scott.  All rights reserved.
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

import xml.dom.minidom
import xml.sax

class SetupError(Exception):
    pass

def main( argv ):
    if sys.platform == 'win32':
        print 'Error: Works for Unix like systems only'
        return 1

    try:
        if argv[1:2] == ['configure']:
            creater = MakeFileCreater()
            return creater.createMakefile( argv )
        elif argv[1:2] == ['help']:
            return setup_help( argv )
        else:
            return setup_help( argv )
    except SetupError, e:
        print 'Error:',str(e)
        return 1

def setup_help( argv ):
    basename = os.path.basename( argv[0] )
    print '''

    Create a makefile for this python and platform
        python %s configure <options>

    Help
        python %s help

    where <options> is one or more of:
        --verbose
        --enable-debug
        --pycxx-dir=<dir>
        --apr-inc-dir=<dir>
        --svn-root-dir=<dir>
        --svn-inc-dir=<dir>
        --svn-lib-dir=<dir>
        --define=<define-string>

''' % (basename, basename)
    return 1

class MakeFileCreater:
    def __init__( self ):
        self.verbose = False
        self.is_mac_os_x = False
        self.is_mac_os_x_fink = False
        self.is_mac_os_x_darwin_ports = False
        self.mac_os_x_version = None

    def node_text( self, all_nodes ):
        all_text = []
        for node in all_nodes:
            if node.nodeType == xml.dom.minidom.Node.TEXT_NODE:
                all_text.append( node.data )
        return ''.join( all_text )

    def cmp_mac_os_x_version( self, version ):
        return cmp( self.mac_os_x_version[0:len(version)], list(version) )

    def detectMacVersion( self ):
        if os.path.exists( '/System/Library/CoreServices/SystemVersion.plist' ):
            dom = xml.dom.minidom.parse( file( '/System/Library/CoreServices/SystemVersion.plist' ) )
            plist = dom.getElementsByTagName( 'plist' )[0]
            plist_dict = plist.getElementsByTagName( 'dict' )[0]
            for key_or_value in plist_dict.childNodes:
                if key_or_value.nodeName == 'key' and self.node_text( key_or_value.childNodes ) == 'ProductVersion':
                    value_node = key_or_value.nextSibling
                    while value_node.nodeType == xml.dom.minidom.Node.TEXT_NODE:
                        value_node = value_node.nextSibling

                    self.mac_os_x_version_string = self.node_text( value_node.childNodes )
                    if self.verbose:
                        print 'Info: Mac OS X Version',self.mac_os_x_version_string
                    self.mac_os_x_version = [int(s) for s in self.mac_os_x_version_string.split('.')]
                    self.is_mac_os_x = True


    def createMakefile( self, argv ):
        self.verbose = '--verbose' in argv
        self.detectMacVersion()

        if self.verbose:
            print 'Info: Creating makefile for python %r' % (sys.version_info,)

        debug_cflags_list = []
        if '--enable-debug' in argv:
            debug_cflags_list.append( '-g' )

        include_dir_list = []

        # add python include dirs
        include_dir_list.append( distutils.sysconfig.get_python_inc() )
        if distutils.sysconfig.get_python_inc() != distutils.sysconfig.get_python_inc( True ):
            include_dir_list.append( distutils.sysconfig.get_python_inc( True ) )
        print 'Info: Found Python include in %s' % ' '.join( include_dir_list )

        # add pycxx include
        pycxx_dir = self.find_pycxx( argv )
        include_dir_list.append( pycxx_dir )
        # add SVN include
        svn_include = self.find_svn_inc( argv )
        include_dir_list.append( svn_include )
        # add APR include
        include_dir_list.append( self.find_apr_inc( argv ) )

        # add source dir
        include_dir_list.append( '.' )

        # get the python CFLAGS
        py_cflags_python_list = distutils.sysconfig.get_config_var('CFLAGS').split()
        # we only want the -D flags, other flags have broken pysvn
        py_cflags_list = []
        for flag in py_cflags_python_list:
            if flag.startswith( '-D' ):
                py_cflags_list.append( flag )

        # add platform specific defines
        if self.is_mac_os_x:
            py_cflags_list.append( '-DDARWIN' )

        # get user supplied defines
        for arg in argv:
            if arg.startswith( '--define=' ):
                py_cflags_list.append( '-D%s' % arg[len('--define-'):] )

        # name of the module including the python version to help
        # ensure that only a matching _pysvn.so for the version of
        # python is imported
        py_cflags_list.append( '-Dinit_pysvn=init_pysvn_%d_%d' % sys.version_info[:2] )
        py_cflags_list.append( '-Dinit_pysvn_d=init_pysvn_%d_%d_d' % sys.version_info[:2] )

        template_values = {
            'pysvn_module_name': '_pysvn_%d_%d' % sys.version_info[:2],
#            'pysvn_module_name': '_pysvn',

            # python executable
            'python_exe':       sys.executable,

            # includes
            'svn_include':      svn_include,
            'includes':         ' '.join( ['-I%s' % include_dir for include_dir in include_dir_list] ),

            # debug_cflags
            'debug_cflags':     ' '.join( debug_cflags_list ),
            
            # py_cflags
            'py_cflags':        ' '.join( py_cflags_list ),

            # add svn lib dir
            'svn_lib_dir':      self.find_svn_lib( argv ),

            # add apr lib dir
            'apr_lib_dir':      self.find_apr_lib( argv ),

            'lib_apr':          self.lib_apr,    # set as a side effect of find_apr_lib

            # pycxx src dir
            'pycxx_dir':        pycxx_dir
            }

        print 'Info: Creating Makefile for Source'

        makefile = file( 'Makefile', 'w' )
        if self.is_mac_os_x:
            # need to figure out the framework dir to use otherwise the latest
            # python framework will be used and not the one matching this python
            var_prefix = distutils.sysconfig.get_config_var('prefix')
            var_ldlibrary = distutils.sysconfig.get_config_var('LDLIBRARY')
            framework_lib = os.path.join( var_prefix, os.path.basename( var_ldlibrary ) )

            if self.cmp_mac_os_x_version( (10,4) ) >= 0:
                if self.verbose:
                    print 'Info: Using Mac OS X 10.4 makefile template'

                # 10.4 needs the libintl.a but 10.3 does not
                template_values['extra_libs'] = '%(svn_lib_dir)s/libintl.a' % template_values
                template_values['frameworks'] = '-framework System %s -framework CoreFoundation -framework Kerberos' % framework_lib
            else:
                if self.verbose:
                    print 'Info: Using Mac OS X 10.3 makefile template'

                template_values['extra_libs'] = ''
                template_values['frameworks'] = '-framework System %s -framework CoreFoundation' % framework_lib

            if self.is_mac_os_x_fink:
                makefile.write( self.makefile_template_macosx_fink % template_values )

            elif self.is_mac_os_x_darwin_ports:
                makefile.write( self.makefile_template_macosx_darwin_ports % template_values )

            else:
                makefile.write( self.makefile_template_macosx % template_values )
        elif sys.platform.startswith('aix'):
            if self.verbose:
                print 'Info: Using AIX makefile template'
            for path in sys.path:
                python_exp = os.path.join(path, 'config', 'python.exp')
                if os.path.exists(python_exp):
                    template_values['python_exp'] = python_exp
                    break
            else:
                python_exp = os.path.abspath(os.path.join(sys.executable, os.path.pardir, os.path.pardir, 
                                                          'lib', 'python2.4', 'config', 'python.exp'))
                if os.path.exists(python_exp):
                    template_values['python_exp'] = python_exp
                else:
                    template_values['python_exp'] = 'python.exp'
            makefile.write( self.makefile_template_aix % template_values )
        elif sys.platform.startswith('linux'):
            if self.verbose:
                print 'Info: Using Linux makefile template'
            makefile.write( self.makefile_template_linux % template_values )
        else:
            if self.verbose:
                print 'Info: Using unix makefile template'
            makefile.write( self.makefile_template % template_values )

        f = file( 'pysvn_common.mak', 'r' )
        makefile.write( f.read() % template_values )
        f.close()
        makefile.close()

        print 'Info: Creating Makefile for Tests'

        makefile = file( '../Tests/Makefile', 'w' )
        makefile.write( self.makefile_tests_template % template_values )
        f = file( '../Tests/pysvn_test_common.mak', 'r' )
        makefile.write( f.read() )
        f.close()
        makefile.close()


    makefile_tests_template = '''#
#	Created by pysvn Extension/Source/setup.py
#       -- makefile_tests_template --
#
PYTHON=%(python_exe)s

#include pysvn_test_common.mak
'''

    makefile_template = '''#
#	Created by pysvn Extension/Source/setup.py
#       -- makefile_template --
#
PYTHON=%(python_exe)s
SVN_INCLUDE=%(svn_include)s
CCC=g++ -c
CCCFLAGS=-Wall -fPIC -fexceptions -frtti %(includes)s %(py_cflags)s %(debug_cflags)s
CC=gcc -c
CCFLAGS=-Wall -fPIC %(includes)s %(debug_cflags)s
PYCXX=%(pycxx_dir)s
LDSHARED=g++ -shared %(debug_cflags)s
LDLIBS=-L%(svn_lib_dir)s -Wl,--rpath -Wl,%(svn_lib_dir)s \
-lsvn_client-1 \
-lsvn_diff-1 \
-lsvn_repos-1 \
 -lcom_err -lresolv -lexpat -lneon

#include pysvn_common.mak
'''

    makefile_template_linux = '''#
#	Created by pysvn Extension/Source/setup.py
#       -- makefile_template_linux --
#
PYTHON=%(python_exe)s
SVN_INCLUDE=%(svn_include)s
CCC=g++ -c
CCCFLAGS=-Wall -fPIC -fexceptions -frtti %(includes)s %(py_cflags)s %(debug_cflags)s
CC=gcc -c
CCFLAGS=-Wall -fPIC %(includes)s %(debug_cflags)s
PYCXX=%(pycxx_dir)s
LDSHARED=g++ -shared %(debug_cflags)s
LDLIBS=-L%(svn_lib_dir)s -Wl,--rpath -Wl,%(svn_lib_dir)s \
-lsvn_client-1 \
-lsvn_diff-1 \
-lsvn_repos-1 \
 -lgssapi_krb5 -lkrb5 -lk5crypto -lkrb5support -lcom_err -lresolv -lexpat -lneon

#include pysvn_common.mak
'''

    makefile_template_aix = '''#
#	Created by pysvn Extension/Source/setup.py
#       -- makefile_template_aix --
#
PYTHON=%(python_exe)s
SVN_INCLUDE=%(svn_include)s
CCC=g++ -c
CCCFLAGS=-Wall -fPIC -fexceptions -frtti %(includes)s %(py_cflags)s %(debug_cflags)s
CC=gcc -c
CCFLAGS=-Wall -fPIC %(includes)s %(debug_cflags)s
PYCXX=%(pycxx_dir)s
LDSHARED=g++ -shared %(debug_cflags)s
LDLIBS=-L%(svn_lib_dir)s \
-lsvn_client-1   \
-lsvn_repos-1    \
-lsvn_subr-1     \
-lsvn_delta-1    \
-lsvn_fs_fs-1    \
-lsvn_fs-1       \
-lsvn_ra_svn-1   \
-lsvn_repos-1    \
-lsvn_ra_local-1 \
-lsvn_ra_dav-1   \
-lsvn_diff-1     \
-lsvn_ra-1       \
-lsvn_wc-1       \
-lapr-0          \
-lneon           \
-laprutil-0      \
-liconv          \
-lexpat          \
-lintl           \
-lpthread        \
-lz              \
-Wl,-bI:%(python_exp)s

#include pysvn_common.mak
'''

    makefile_template_macosx = '''#
#	Created by pysvn Extension/Source/setup.py
#       -- makefile_template_macosx --
#
PYTHON=%(python_exe)s
SVN_INCLUDE=%(svn_include)s
CCC=g++ -c
CCCFLAGS=-Wall -Wno-long-double -fPIC -fexceptions -frtti %(includes)s %(py_cflags)s %(debug_cflags)s
CC=gcc -c
CCFLAGS=-Wall -Wno-long-double -fPIC %(includes)s %(debug_cflags)s
PYCXX=%(pycxx_dir)s
LDSHARED=g++ -bundle %(debug_cflags)s -u _PyMac_Error %(frameworks)s
LDLIBS=-L%(svn_lib_dir)s \
-L%(apr_lib_dir)s \
-lsvn_client-1 \
-lsvn_repos-1 \
-lsvn_wc-1 \
-lsvn_fs-1 \
-lsvn_subr-1 \
-lsvn_diff-1 \
-l%(lib_apr)s
#include pysvn_common.mak
'''

    makefile_template_macosx_fink = '''#
#	Created by pysvn Extension/Source/setup.py
#       -- makefile_template_macosx_fink --
#
PYTHON=%(python_exe)s
SVN_INCLUDE=%(svn_include)s
CCC=g++ -c
CCCFLAGS=-Wall -Wno-long-double -fPIC -fexceptions -frtti %(includes)s %(py_cflags)s %(debug_cflags)s
CC=gcc -c
CCFLAGS=-Wall -Wno-long-double -fPIC %(includes)s %(debug_cflags)s
PYCXX=%(pycxx_dir)s
LDSHARED=g++ -bundle %(debug_cflags)s -u _PyMac_Error %(frameworks)s
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
%(apr_lib_dir)s/libaprutil-0.a \
%(apr_lib_dir)s/libapr-0.a \
%(svn_lib_dir)s/libneon.a \
%(svn_lib_dir)s/libssl.a \
%(svn_lib_dir)s/libcrypto.a \
%(svn_lib_dir)s/libexpat.a \
%(svn_lib_dir)s/libxml2.a \
%(svn_lib_dir)s/libdb-4.3.a \
 -lintl.3 -liconv.2 -lz

#include pysvn_common.mak
'''

    makefile_template_macosx_darwin_ports = '''#
#	Created by pysvn Extension/Source/setup.py
#       -- makefile_template_macosx_darwin_ports --
#
PYTHON=%(python_exe)s
SVN_INCLUDE=%(svn_include)s
CCC=g++ -c
CCCFLAGS=-Wall -Wno-long-double -fPIC -fexceptions -frtti %(includes)s %(py_cflags)s %(debug_cflags)s
CC=gcc -c
CCFLAGS=-Wall -Wno-long-double -fPIC %(includes)s %(debug_cflags)s
PYCXX=%(pycxx_dir)s
LDSHARED=g++ -bundle %(debug_cflags)s -u _PyMac_Error %(frameworks)s
LDLIBS= \
%(svn_lib_dir)s/libsvn_client-1.a \
%(svn_lib_dir)s/libsvn_subr-1.a \
%(svn_lib_dir)s/libsvn_wc-1.a \
%(svn_lib_dir)s/libsvn_ra-1.a \
%(svn_lib_dir)s/libsvn_ra_dav-1.a \
%(svn_lib_dir)s/libsvn_ra_local-1.a \
%(svn_lib_dir)s/libsvn_ra_svn-1.a \
%(svn_lib_dir)s/libsvn_delta-1.a \
%(svn_lib_dir)s/libsvn_fs-1.a \
%(svn_lib_dir)s/libsvn_fs_fs-1.a \
%(svn_lib_dir)s/libsvn_fs_base-1.a \
%(svn_lib_dir)s/libsvn_repos-1.a \
%(svn_lib_dir)s/libsvn_diff-1.a \
%(apr_lib_dir)s/libaprutil-1.a \
%(svn_lib_dir)s/libneon.a \
%(svn_lib_dir)s/libssl.a \
%(svn_lib_dir)s/libcrypto.a \
%(svn_lib_dir)s/libexpat.a \
%(svn_lib_dir)s/libiconv.a \
%(svn_lib_dir)s/libdb-4.3.a \
%(extra_libs)s \
-L%(apr_lib_dir)s \
-l%(lib_apr)s \
-lz

#include pysvn_common.mak
'''

    def find_pycxx( self, argv ):
        return self.find_dir( argv,
                    'PyCXX include',
                    '--pycxx-dir=',
                    None,
                    [   '../Import/pycxx-5.4.0'],
                    'CXX/Version.hxx' )

    def find_svn_inc( self, argv ):
        return self.find_dir( argv,
                    'SVN include',
                    '--svn-inc-dir=',
                    'include/subversion-1',
                    [
                        '/opt/local/include/subversion-1',      # Darwin - darwin ports
                        '/sw/include/subversion-1',             # Darwin - Fink
                        '/usr/include/subversion-1',            # typical Linux
                        '/usr/local/include/subversion-1',      # typical *BSD
                        '/usr/pkg/include/subversion-1',        # netbsd
                    ],
                    'svn_client.h' )

    def find_svn_lib( self, argv ):
        dir = self.find_dir( argv,
                    'SVN library',
                    '--svn-lib-dir=',
                    'lib',
                    [
                        '/opt/local/lib',                       # Darwin - darwin ports
                        '/sw/lib',                              # Darwin - Fink
                        '/usr/lib64',                           # typical 64bit Linux
                        '/usr/lib',                             # typical Linux
                        '/usr/local/lib64',                     # typical 64bit Linux
                        '/usr/local/lib',                       # typical *BSD
                        '/usr/pkg/lib',                         # netbsb
                    ],
                    self.is_mac_os_x and 'libsvn_client-1.dylib' or 'libsvn_client-1.so' )
        # if we are using the Fink SVN then remember this
        self.is_mac_os_x_fink = dir.startswith( '/sw/' )
        self.is_mac_os_x_darwin_ports = dir.startswith( '/opt/local/' )
        return dir

    def find_apr_inc( self, argv ):
        last_exception = None
        for apr_ver in ['apr-1', 'apr-0']:
            try:
                return self.find_dir( argv,
                    'APR include',
                    '--apr-inc-dir=',
                    'include/%s' % apr_ver,
                    [
                    ],
                    'apr.h' )
            except SetupError:
                pass

        for apr_ver in ['apr-1', 'apr-0']:
            try:
                return self.find_dir( argv,
                    'APR include',
                    '--apr-inc-dir=',
                    None,
                    [
                        '/opt/local/include/%s' % apr_ver,      # Darwin - darwin ports
                        '/sw/include/%s' % apr_ver,             # Darwin - fink
                        '/usr/include/%s' % apr_ver,            # typical Linux
                        '/usr/local/apr/include/%s' % apr_ver,  # Mac OS X www.metissian.com
                        '/usr/pkg/include/%s' % apr_ver,        # netbsd
                        '/usr/include/apache2',                 # alternate Linux
                        '/usr/include/httpd',                   # alternate Linux
                        '/usr/local/include/apr0',              # typical *BSD
                        '/usr/local/include/apache2',           # alternate *BSD
                    ],
                    'apr.h' )
            except SetupError, e:
                last_exception = e
        raise last_exception

    def find_apr_lib( self, argv ):
        last_exception = None
        for apr_libname, self.lib_apr in [
                            (self.is_mac_os_x and 'libapr-1.dylib' or 'libapr-1.so', 'apr-1'),
                            (self.is_mac_os_x and 'libapr-0.dylib' or 'libapr-0.so', 'apr-0')]:
            try:
                return self.find_dir( argv,
                    'APR library',
                    '--apr-lib-dir=',
                    'lib',
                    [],
                    apr_libname )
            except SetupError:
                pass

        for apr_libname, self.lib_apr in [
                            (self.is_mac_os_x and 'libapr-1.dylib' or 'libapr-1.so', 'apr-1'),
                            (self.is_mac_os_x and 'libapr-0.dylib' or 'libapr-0.so', 'apr-0')]:
            try:
                return self.find_dir( argv,
                    'APR library',
                    '--apr-lib-dir=',
                    None,
                    [
                        '/opt/local/lib',                       # Darwin - darwin ports
                        '/sw/lib',                              # Darwin - fink
                        '/usr/lib64',                           # typical 64bit Linux
                        '/usr/lib',                             # typical Linux
                        '/usr/local/lib64',                     # typical 64bit Linux
                        '/usr/local/lib',                       # typical *BSD
                        '/usr/local/apr/lib',                   # Mac OS X www.metissian.com
                        '/usr/pkg/lib',                         # netbsd
                    ],
                    apr_libname )
            except SetupError, e:
                last_exception = e
        raise last_exception


    def find_dir( self, argv, name, kw, svn_root_suffix, base_dir_list, check_file ):
        dirname = self.__find_dir( argv, name, kw, svn_root_suffix, base_dir_list, check_file )
        print 'Info: Found %14.14s in %s' % (name, dirname)
        return dirname

    def __find_dir( self, argv, name, kw, svn_root_suffix, base_dir_list, check_file ):
        # override the base_dir_list from the command line kw
        svn_root_dir = None
        for arg in argv[2:]:
            if arg[0:len(kw)] == kw:
                base_dir_list = [arg[len(kw):]]
            elif( arg[0:len('--svn-root-dir=')] == '--svn-root-dir='
            and svn_root_suffix is not None ):
                base_dir_list = ['%s/%s' % (arg[len('--svn-root-dir='):], svn_root_suffix)]

        # expect to find check_file in one of the dir
        for dir in base_dir_list:
            full_check_file = os.path.join( dir, check_file )
            if self.verbose:
                print 'Info: Checking for %s in %s' % (name, full_check_file)
            if os.path.exists( full_check_file ):
                return os.path.abspath( dir )

        raise SetupError, 'cannot find %s %s - use %s' % (name, check_file, kw)

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
