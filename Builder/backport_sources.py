#
#   backport_sources.py
#
#   fixup the python sources to work with python2.5 and earlier
#
import os
import sys

all_files = []

def walk( dirname, all_files ):
    for filename in os.listdir( dirname ):
        filename = os.path.join( dirname, filename )

        if os.path.isdir( filename ):    
            walk( filename, all_files )

        elif filename.endswith( '.py' ):
            all_files.append( filename )

        elif filename.endswith( '.py.template' ):
            all_files.append( filename )


def fixup_source( filename ):
    f = open( filename, 'r' )
    all_lines = f.readlines()
    f.close()

    replaced = False

    for index in range( len( all_lines ) ):
        if( all_lines[ index ].strip().startswith( 'except ' )
        and ' as e:' in all_lines[ index ] ):
            replaced = True
            print( filename )
            print( 'Before: %r' % all_lines[ index ] )
            all_lines[ index ] = all_lines[ index ].replace( ' as e:', ', e:' )
            print( ' After: %r' % all_lines[ index ] )

    if replaced:
        print( 'Updating %s' % filename )
        f = open( filename, 'w' )
        f.write( ''.join( all_lines ) )
        f.close()

walk( sys.argv[1], all_files )

all_files.sort()
for filename in all_files:
    fixup_source( filename )


