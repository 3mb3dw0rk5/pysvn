#
#	BUild the example on Linux for version Python 2.3
#
PYCXXDIR=$(HOME)/Latest/PyCXX/CXX
SVNCPPDIR=$(HOME)/subversion/Linux_rh90/rapidsvn/src/svncpp
SVNINCLUDE=/usr/include
SVNDIR=$(HOME)/subversion/Linux_rh90/subversion-0.32.1
APRINCLUDE=/usr/local/apache2/include
CCC=g++ -c
CCCFLAGS=-fPIC -fexceptions -frtti -I/usr/include/python2.3 -I$(PYCXXDIR) -I$(SVNCPPDIR) -I$(SVNINCLUDE)/subversion-1 -I$(APRINCLUDE) -I.
LDSHARED=g++ -shared
LDLIBS=$(SVNCPPDIR)/.libs/libsvncpp.a \
$(SVNDIR)/subversion/libsvn_client/.libs/libsvn_client-1.a \
$(SVNDIR)/subversion/libsvn_wc/.libs/libsvn_wc-1.a \
$(SVNDIR)/subversion/libsvn_ra/.libs/libsvn_ra-1.a \
$(SVNDIR)/subversion/libsvn_ra_dav/.libs/libsvn_ra_dav-1.a \
$(SVNDIR)/subversion/libsvn_ra_local/.libs/libsvn_ra_local-1.a \
$(SVNDIR)/subversion/libsvn_ra_svn/.libs/libsvn_ra_svn-1.a \
$(SVNDIR)/subversion/libsvn_diff/.libs/libsvn_diff-1.a \
$(SVNDIR)/subversion/libsvn_repos/.libs/libsvn_repos-1.a \
$(SVNDIR)/subversion/libsvn_fs/.libs/libsvn_fs-1.a \
$(SVNDIR)/subversion/libsvn_delta/.libs/libsvn_delta-1.a \
$(SVNDIR)/subversion/libsvn_subr/.libs/libsvn_subr-1.a \
$(SVNDIR)/apr-util/.libs/libaprutil-0.a \
$(SVNDIR)/apr/.libs/libapr-0.a \
$(SVNDIR)/db/dist/.libs/libdb-4.0.a -lneon -lexpat -lcrypt


PYTHON=python2.3

include pysvn_common.mak
