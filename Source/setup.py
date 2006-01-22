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

    where <options> is one or more of:
        --verbose
        --pycxx-dir=<dir>
        --apr-inc-dir=<dir>
        --svn-inc-dir=<dir>
        --svn-lib-dir=<dir>

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

        template_values = {
            # python executable
            'python_exe':       sys.executable,

            # includes
            'svn_include':      svn_include,
            'includes':         ' '.join( ['-I%s' % include_dir for include_dir in include_dir_list] ),

            # add svn lib dir
            'svn_lib_dir':      self.find_svn_lib( argv ),

            # add apr lib dir
            'apr_lib_dir':      self.find_apr_lib( argv ),

            # pycxx src dir
            'pycxx_dir':        pycxx_dir
            }

        if self.verbose:
            print 'Info: Creating Makefile'
        makefile = file( 'Makefile', 'w' )
        if self.is_mac_os_x:
            # need to figure out the framework dir to use otherwise the latest
            # python framework will be used and not the one matching this python
            framework_dir = distutils.sysconfig.get_python_inc().split('/Python.framework')[0]

            if self.cmp_mac_os_x_version( (10,4) ) >= 0:
                if self.verbose:
                    print 'Info: Using Mac OS X 10.4 makefile template'
                template_values['frameworks'] = '-F%s -framework System -framework Python -framework CoreFoundation -framework Kerberos' % framework_dir
            else:
                if self.verbose:
                    print 'Info: Using Mac OS X 10.3 makefile template'
                template_values['frameworks'] = '-F%s -framework System -framework Python -framework CoreFoundation' % framework_dir
            if self.is_mac_os_x_fink:
                makefile.write( self.makefile_template_macosx_fink % template_values )
            elif self.is_mac_os_x_darwin_ports:
                makefile.write( self.makefile_template_macosx_darwin_ports % template_values )
            else:
                makefile.write( self.makefile_template_macosx % template_values )
        else:
            if self.verbose:
                print 'Info: Using unix makefile template'
            makefile.write( self.makefile_template % template_values )
        makefile.close()

    makefile_template = '''#
#	Created by pysvn Extension/Source/setup.py
#
PYTHON=%(python_exe)s
SVN_INCLUDE=%(svn_include)s
CCC=g++ -c
CCCFLAGS=-Wall -fPIC -fexceptions -frtti %(includes)s
CC=gcc -c
CCFLAGS=-Wall -fPIC %(includes)s
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
SVN_INCLUDE=%(svn_include)s
CCC=g++ -c
CCCFLAGS=-Wall -Wno-long-double -fPIC -fexceptions -frtti %(includes)s
CC=gcc -c
CCFLAGS=-Wall -Wno-long-double -fPIC %(includes)s
PYCXX=%(pycxx_dir)s
LDSHARED=g++ -bundle -g -u _PyMac_Error %(frameworks)s
LDLIBS=-L%(svn_lib_dir)s \
-L%(apr_lib_dir)s \
-lsvn_client-1 \
-lsvn_repos-1 \
-lsvn_wc-1 \
-lsvn_fs-1 \
-lsvn_subr-1 \
-lapr-0
include pysvn_common.mak
'''

    makefile_template_macosx_fink = '''#
#	Created by pysvn Extension/Source/setup.py
#
PYTHON=%(python_exe)s
SVN_INCLUDE=%(svn_include)s
CCC=g++ -c
CCCFLAGS=-Wall -Wno-long-double -fPIC -fexceptions -frtti %(includes)s
CC=gcc -c
CCFLAGS=-Wall -Wno-long-double -fPIC %(includes)s
PYCXX=%(pycxx_dir)s
LDSHARED=g++ -bundle -g -u _PyMac_Error %(frameworks)s
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
include pysvn_common.mak
'''

    makefile_template_macosx_darwin_ports = '''#
#	Created by pysvn Extension/Source/setup.py
#
PYTHON=%(python_exe)s
SVN_INCLUDE=%(svn_include)s
CCC=g++ -c
CCCFLAGS=-Wall -Wno-long-double -fPIC -fexceptions -frtti %(includes)s
CC=gcc -c
CCFLAGS=-Wall -Wno-long-double -fPIC %(includes)s
PYCXX=%(pycxx_dir)s
LDSHARED=g++ -bundle -g -u _PyMac_Error %(frameworks)s
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
-L%(apr_lib_dir)s \
-lapr-1 -lz
include pysvn_common.mak
'''

    def find_pycxx( self, argv ):
        return self.find_dir( argv,
                    'PyCXX include',
                    '--pycxx-dir=',
                    [   '../Import/pycxx_5_3_5'],
                    'CXX/Version.hxx' )

    def find_svn_inc( self, argv ):
        return self.find_dir( argv,
                    'SVN include',
                    '--svn-inc-dir=',
                    [
                        '/opt/local/include/subversion-1',      # Darwin - darwin ports
                        '/sw/include/subversion-1',             # Darwin - Fink
                        '/usr/include/subversion-1',            # typical Linux
                        '/usr/local/include/subversion-1',      # typical *BSD
                    ],
                    'svn_client.h' )

    def find_svn_lib( self, argv ):
        dir = self.find_dir( argv,
                    'SVN library',
                    '--svn-lib-dir=',
                    [
                        '/opt/local/lib',                       # Darwin - darwin ports
                        '/sw/lib',                              # Darwin - Fink
                        '/usr/lib',                             # typical Linux
                        '/usr/lib64',                           # typical 64bit Linux
                        '/usr/local/lib',                       # typical *BSD
                    ],
                    self.is_mac_os_x and 'libsvn_client-1.dylib' or 'libsvn_client-1.so' )
        # if we are using the Fink SVN then remember this
        self.is_mac_os_x_fink = dir.startswith( '/sw/' )
        self.is_mac_os_x_darwin_ports = dir.startswith( '/opt/local/' )
        return dir

    def find_apr_inc( self, argv ):
        return self.find_dir( argv,
                    'APR include',
                    '--apr-inc-dir=',
                    [
                        '/opt/local/include/apr-1',             # Darwin - darwin ports
                        '/sw/include/apr-0',                    # Darwin - fink
                        '/usr/include/apr-0',                   # typical Linux
                        '/usr/include/apache2',                 # alternate Linux
                        '/usr/include/httpd',                   # alternate Linux
                        '/usr/local/include/apr0',              # typical *BSD
                        '/usr/local/include/apache2',           # alternate *BSD
                        '/usr/local/apr/include/apr-0',         # Mac OS X www.metissian.com
                    ],
                    'apr.h' )

    def find_apr_lib( self, argv ):
        last_exception = None
        for apr_libname in [self.is_mac_os_x and 'libapr-1.dylib' or 'libapr-1.so',
                            self.is_mac_os_x and 'libapr-0.dylib' or 'libapr-0.so']:
            try:
                return self.find_dir( argv,
                    'APR library',
                    '--apr-lib-dir=',
                    [
                        '/opt/local/lib',                       # Darwin - darwin ports
                        '/sw/lib',                              # Darwin - fink
                        '/usr/lib',                             # typical Linux
                        '/usr/local/lib',                       # typical *BSD
                        '/usr/local/apr/lib',                   # Mac OS X www.metissian.com
                    ],
                    apr_libname )
            except SetupError, e:
                last_exception = e
        raise e


    def find_dir( self, argv, name, kw, base_dir_list, check_file=None ):
        dir = self.__find_dir( argv, name, kw, base_dir_list, check_file )
        print 'Info: Found %14.14s in %s' % (name, dir)
        return dir

    def __find_dir( self, argv, name, kw, base_dir_list, check_file=None ):
        for arg in argv[2:]:
            if arg[0:len(kw)] == kw:
                return arg[len(kw):]

        # expect to find the pycxx in this source tree
        for dir in base_dir_list:
            full_check_file = os.path.join( dir, check_file )
            if self.verbose:
                print 'Info: Checking for %s in %s' % (name, full_check_file)
            if os.path.exists( full_check_file ):
                return os.path.abspath( dir )

        raise SetupError, 'cannot find %s - use %s' % (name, kw)

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
