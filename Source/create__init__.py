#
#   create__init__.py
#
import sys
import os

svn_include = sys.argv[1]
init_template = sys.argv[2]
init_output = sys.argv[3]

pymaj, pymin, pypat, _, _ = sys.version_info
python_version_string = '%d.%d.%d' % (pymaj, pymin, pypat)

print 'Info: Creating __init__.py for python version %s' % python_version_string
pysvn__init__file_contents = file( init_template ).readlines()
block_begin_index = pysvn__init__file_contents.index( '### IMPORT BLOCK BEGIN\n' )
block_end_index = pysvn__init__file_contents.index( '### IMPORT BLOCK END\n' ) + 1

pysvn__init__file_contents[ block_begin_index:block_end_index ] = [
    '    import _pysvn_%d_%d\n' % (pymaj, pymin),
    '    _pysvn = _pysvn_%d_%d\n' % (pymaj, pymin),
    ]

f = open( init_output, 'w' )
f.write( ''.join( pysvn__init__file_contents ) )
f.close()

f = open( 'generate_svn_error_codes.cpp', 'w' )
svn_err_file = open( os.path.join( svn_include, 'svn_error_codes.h' ), 'r' )

f.write( '#include <stdlib.h>\n' )
f.write( '#include <stdio.h>\n' )
f.write( '#include "svn_error_codes.h"\n' )
f.write( 'int main( int argc, char **argv )\n' )
f.write( '{\n' )
f.write( '    printf( "\\n" );\n' )
f.write( '    printf( "class svn_err:\\n" );\n' )

emit = False
for line in svn_err_file:
    if line == 'SVN_ERROR_START\n':
        emit = True

    if emit and 'SVN_ERRDEF(' in line:
        symbol = line.split( 'SVN_ERRDEF(' )[1].split( ',' )[0]
        f.write( '    printf( "    %s=%%d\\n", %s );\n' % (symbol.lower()[len('SVN_ERR_'):], symbol) )

f.write( '    return 0;\n' )
f.write( '}\n' )
f.close()
