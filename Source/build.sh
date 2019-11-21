#!/bin/bash
set -e

if [ "$1" != "" ]
then
    PY_VER=$1
fi

if [ "$2" != "" ]
then
    SVN_VER=$2
fi

export MACOSX_DEPLOYMENT_TARGET=10.9

case "$(uname)" in
Linux)
    python${PY_VER} setup.py configure \
        --enable-debug \
        --verbose
    ;;

Darwin)
    python${PY_VER} setup.py configure \
        --distro-dir=/usr/local/svn-${SVN_VER} \
        --distro-dir=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr \
        --arch=x86_64 \
        --pycxx-dir=${BUILDER_TOP_DIR}/Import/pycxx-${PYCXX_VER} \
        --verbose
    ;;
*)
    echo "Error: need support for $(uname)"
    exit 1
    ;;

esac

make clean
make all

cd ../Tests
make clean
make all
