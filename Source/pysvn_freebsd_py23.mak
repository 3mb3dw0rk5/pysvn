#
#	BUild the example on FreeBSD for version Python 2.3
#
CCC=g++ -c
CCCFLAGS=-fPIC -fexceptions -frtti -I/usr/local/include/$(PYDIR) -I$(PYCXX) -I$(SVN_INC) -I$(APR_INC) -I.
LDSHARED=g++ -shared
LDLIBS=-L$(SVN_LIB) \
-lsvn_client-1

include pysvn_common.mak
