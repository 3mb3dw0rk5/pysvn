#
#	Build the example on Mac OS X for version Python 2.3
#
PYROOT=/System/Library/Frameworks/Python.framework/Versions/2.3
CCC=g++ -c -g
CCCFLAGS=-Wno-long-double -fPIC -fexceptions -frtti -I$(PYROOT)/Headers -I$(PYCXX) -I$(SVN_INC) -I$(APR_INC) -I.
LDSHARED=g++ -bundle -g -u _PyMac_Error -framework System -framework Python
LDLIBS= \
$(SVN_LIB)/libsvn_client-1.a \
$(SVN_LIB)/libsvn_subr-1.a \
$(SVN_LIB)/libsvn_wc-1.a \
$(SVN_LIB)/libsvn_ra-1.a \
$(SVN_LIB)/libsvn_ra_dav-1.a \
$(SVN_LIB)/libsvn_ra_local-1.a \
$(SVN_LIB)/libsvn_ra_svn-1.a \
$(SVN_LIB)/libsvn_delta-1.a \
$(SVN_LIB)/libsvn_repos-1.a \
$(SVN_LIB)/libsvn_fs-1.a \
$(SVN_LIB)/libsvn_fs_fs-1.a \
$(SVN_LIB)/libsvn_fs_base-1.a \
$(SVN_LIB)/libsvn_diff-1.a \
$(SVN_LIB)/libaprutil-0.a \
$(SVN_LIB)/libapr-0.a \
$(SVN_LIB)/libneon.a \
$(SVN_LIB)/libssl.a \
$(SVN_LIB)/libcrypto.a \
$(SVN_LIB)/libexpat.a \
$(SVN_LIB)/libxml2.a \
$(SVN_LIB)/libdb-4.2.a \
$(SVN_LIB)/libintl.a \
\
 -liconv -lz

include pysvn_common.mak
