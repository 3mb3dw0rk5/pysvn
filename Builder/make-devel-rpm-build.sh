#!/bin/bash -x
#
#   make-devel-rpm-build.sh
#
#   Create a source kit and copy it and the rpm spec file to ~/rpmbuild locations
#
. ./version.info
V=${MAJOR}.${MINOR}.${PATCH}
rm -rf /tmp/pysvn-${V}

svn export --quiet .. /tmp/pysvn-${V}
BUILD=$( svnversion .. )
cat <<EOF >/tmp/pysvn-${V}/Builder/version.info
MAJOR=${MAJOR}
MINOR=${MINOR}
PATCH=${PATCH}
BUILD=${BUILD}
EOF
echo Info: Creating source kit...
mkdir -p /tmp/rpmbuild/SOURCES
mkdir -p /tmp/rpmbuild/SPECS

tar czf /tmp/rpmbuild/SOURCES/pysvn-${V}.tar.gz -C /tmp pysvn-${V}
echo Info: Running rpmbuild
cd /tmp/rpmbuild/SPECS
rpmbuild -ba pysvn.spec
