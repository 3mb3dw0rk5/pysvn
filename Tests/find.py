#
#   find.py
#
#   It turns out that Mac OS X and linux sort do not use the same
#   ordering. Python allows for a portable and reliable.
#
import os
import sys

# copied from python 2.3 as we need to run on 2.2 that does not have walk
def walk(top, topdown=True, onerror=None):
    from os.path import join, isdir, islink

    # We may not have read permission for top, in which case we can't
    # get a list of the files the directory contains.  os.path.walk
    # always suppressed the exception then, rather than blow up for a
    # minor reason when (say) a thousand readable directories are still
    # left to visit.  That logic is copied here.
    try:
        # Note that listdir and error are globals in this module due
        # to earlier import-*.
        names = os.listdir(top)
    except error, err:
        if onerror is not None:
            onerror(err)
        return

    dirs, nondirs = [], []
    for name in names:
        if isdir(join(top, name)):
            dirs.append(name)
        else:
            nondirs.append(name)

    if topdown:
        yield top, dirs, nondirs
    for name in dirs:
        path = join(top, name)
        if not islink(path):
            for x in walk(path, topdown, onerror):
                yield x
    if not topdown:
        yield top, dirs, nondirs


all_files = []
for root, dirs, files in walk( sys.argv[1] ):
    for name in files:
        all_files.append( os.path.join( root, name ) )

all_files.sort()
for file in all_files:
    print file
