#!/bin/echo Usage: . $0
export WORKDIR=$(cd ..;pwd)

export SVNVER=0.36.0
export PYCXX=${WORKDIR}/Import/pycxx_5_2_0
export PYTHON=/usr/bin/python2.3
export PYDIR=python2.3
export PYLIB=python23
export SVN_INC=/usr/include/subversion-1
export SVN_LIB=/usr/lib
export APR_INC=/usr/include/apr-0
export APR_LIB=/usr/lib
export SVNCPP_INC=${HOME}/subversion/Linux_rh90/rapidsvn/include
export SVNCPP_LIB=${HOME}/subversion/Linux_rh90/rapidsvn/src/svncpp/.libs

# prove the python version selected
${PYTHON} -c "import sys;print 'Info: Python Version',sys.version"
