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

    where <options> is one of more of:
        --pycxx-dir=<dir>
        --apr-inc-dir=<dir>
        --svn-inc-dir=<dir>
        --svn-lib-dir=<dir>

''' % (basename, basename)
    return 1

class MakeFileCreater:
    def __init__( self ):
        self.is_mac_os_x = False
        self.mac_os_x_version = None
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
                    print 'Info: Mac OS X Version',self.mac_os_x_version_string
                    self.mac_os_x_version = [int(s) for s in self.mac_os_x_version_string.split('.')]
                    self.is_mac_os_x = True

    def node_text( self, all_nodes ):
        all_text = []
        for node in all_nodes:
            if node.nodeType == xml.dom.minidom.Node.TEXT_NODE:
                all_text.append( node.data )
        return ''.join( all_text )

    def cmp_mac_os_x_version( self, version ):
        return cmp( self.mac_os_x_version[0:len(version)], list(version) )

    def createMakefile( self, argv ):
        print 'Info: Creating makefile for python %r' % (sys.version_info,)

        include_dir_list = []

        # add python include dirs
        include_dir_list.append( distutils.sysconfig.get_python_inc() )
        if distutils.sysconfig.get_python_inc() != distutils.sysconfig.get_python_inc( True ):
            include_dir_list.append( distutils.sysconfig.get_python_inc( True ) )
        print 'Info: Python includes %s' % ' '.join( include_dir_list )

        # add pycxx include
        pycxx_dir = self.find_pycxx( argv )
        include_dir_list.append( pycxx_dir )
        # add SVN include
        include_dir_list.append( self.find_svn_inc( argv ) )
        # add APR include
        include_dir_list.append( self.find_apr_inc( argv ) )

        # add source dir
        include_dir_list.append( '.' )

        template_values = {
            # python executable
            'python_exe':       sys.executable,

            # includes
            'includes':         ' '.join( ['-I%s' % include_dir for include_dir in include_dir_list] ),

            # add svn lib dir
            'svn_lib_dir':      self.find_svn_lib( argv ),

            # pycxx src dir
            'pycxx_dir':        pycxx_dir
            }

        print 'Info: Creating Makefile'
        makefile = file( 'Makefile', 'w' )
        if self.is_mac_os_x:
            if self.cmp_mac_os_x_version( (10,4) ) >= 0:
                print 'Info: Using Mac OS X 10.4 makefile template'
                template_values['frameworks'] = '-framework System -framework Python -framework CoreFoundation -framework Kerberos'
            else:
                print 'Info: Using Mac OS X 10.3 makefile template'
                template_values['frameworks'] = '-framework System -framework Python -framework CoreFoundation'
            makefile.write( self.makefile_template_macosx % template_values )
        else:
            print 'Info: Using unix makefile template'
            makefile.write( self.makefile_template % template_values )
        makefile.close()

    makefile_template = '''#
#	Created by pysvn Extension/Source/setup.py
#
PYTHON=%(python_exe)s
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
%(svn_lib_dir)s/libaprutil-0.a \
%(svn_lib_dir)s/libapr-0.a \
%(svn_lib_dir)s/libneon.a \
%(svn_lib_dir)s/libssl.a \
%(svn_lib_dir)s/libcrypto.a \
%(svn_lib_dir)s/libexpat.a \
%(svn_lib_dir)s/libxml2.a \
%(svn_lib_dir)s/libdb-4.3.a \
-L%(svn_lib_dir)s\
 -lintl.3 -liconv.2 -lz
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
                    [   '/usr/include/subversion-1',            # typical Linux
                        '/usr/local/include/subversion-1',      # typical *BSD
                        '/sw/include/subversion-1'],            # Darwin
                    'svn_client.h' )

    def find_svn_lib( self, argv ):
        return self.find_dir( argv,
                    'SVN library',
                    '--svn-lib-dir=',
                    [   '/usr/lib',                             # typical Linux
                        '/usr/local/lib',                       # typical *BSD
                        '/sw/lib'],                             # Darwin
                    self.is_mac_os_x and 'libsvn_client-1.dylib' or 'libsvn_client-1.so' )

    def find_apr_inc( self, argv ):
        return self.find_dir( argv,
                    'APR include',
                    '--apr-inc-dir=',
                    [   '/usr/include/apr-0',                   # typical Linux
                        '/usr/include/apache2',                 # alternate Linux
                        '/usr/local/include/apr0',              # typical *BSD
                        '/usr/local/include/apache2',           # alternate *BSD
                        '/sw/include/apr-0'],                   # Darwin
                    'apr.h' )

    def find_apr_lib( self, argv ):
        return self.find_dir( argv,
                    'APR library',
                    '--apr-lib-dir=',
                    [   '/usr/lib',                             # typical Linux
                        '/usr/local/lib',                       # typical *BSD
                        '/sw/lib'],                             # Darwin
                    self.is_mac_os_x and 'libapr-0.dylib' or 'libapr-0.so' )

    def find_dir( self, argv, name, kw, base_dir_list, check_file=None ):
        dir = self.__find_dir( argv, name, kw, base_dir_list, check_file )
        print 'Info: Found',name,'in',dir
        return dir

    def __find_dir( self, argv, name, kw, base_dir_list, check_file=None ):
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
