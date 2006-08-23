'''
 ====================================================================
 Copyright (c) 2003-2006 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================
'''

# At the moment this package is only a kitting convenience
try:
    from _pysvn import *
except ImportError, e:
    # check for common installation errors that show up as ImportError
    if ': undefined symbol:' in str(e):
        raise ImportError, 'pysvn was built against newer (svn, apr, etc.) libraries then the ones installed on this system. %s' % str(e)
    else:
        raise

# An indication that you are interested in the @c kind field
SVN_DIRENT_KIND        = 0x00001

# An indication that you are interested in the @c size field
SVN_DIRENT_SIZE        = 0x00002

# An indication that you are interested in the @c has_props field
SVN_DIRENT_HAS_PROPS   = 0x00004

# An indication that you are interested in the @c created_rev field
SVN_DIRENT_CREATED_REV = 0x00008

# An indication that you are interested in the @c time field
SVN_DIRENT_TIME        = 0x00010

# An indication that you are interested in the @c last_author field
SVN_DIRENT_LAST_AUTHOR = 0x00020

# A combination of all the dirent fields
SVN_DIRENT_ALL         = 0xffffffffl
