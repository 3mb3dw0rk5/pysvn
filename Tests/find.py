#
#   find.py
#
#   It turns out that Mac OS X and linux sort do not use the same
#   ordering. Python allows for a portable and reliable.
#
import os
import sys

all_files = []
for root, dirs, files in os.walk( sys.argv[1] ):
    for name in files:
        all_files.append( os.path.join( root, name ) )

all_files.sort()
for file in all_files:
    print file
