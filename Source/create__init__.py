#
#   create__init__.py
#
import sys

pymaj, pymin, pypat, _, _ = sys.version_info
python_version_string = '%d.%d.%d' % (pymaj, pymin, pypat)

print 'Info: Creating __init__.py for python version %s' % python_version_string
pysvn__init__file_contents = file( sys.argv[1] ).readlines()
block_begin_index = pysvn__init__file_contents.index( '### IMPORT BLOCK BEGIN\n' )
block_end_index = pysvn__init__file_contents.index( '### IMPORT BLOCK END\n' ) + 1

pysvn__init__file_contents[ block_begin_index:block_end_index ] = [
    '    import _pysvn_%d_%d\n' % (pymaj, pymin),
    '    _pysvn = _pysvn_%d_%d\n' % (pymaj, pymin),
    ]

f = file( sys.argv[2], 'w' )
f.write( ''.join( pysvn__init__file_contents ) )
f.close()
