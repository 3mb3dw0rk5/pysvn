#
#	BUild the example on Linux for version Python 2.3
#
CCC=g++ -g -c
CCCFLAGS=-fPIC -fexceptions -frtti -I/usr/include/$(PYDIR) -I$(PYCXX) -I$(SVNCPP_INC) -I$(SVN_INC) -I$(APR_INC) -I.
LDSHARED=g++ -shared
LDLIBS=-L$(SVN_LIB) \
$(SVNCPP_LIB)/libsvncpp.a \
-lsvn_client-1

include pysvn_common.mak
