#!/bin/echo Usage: . $0
export WORKDIR=$(cd ..;pwd)

# default to 2.3 if no value in $1 and $2
export PY_MAJ=${1:-2}
export PY_MIN=${2:-3}
export SVNVER=1.0.0
export PYCXX=${WORKDIR}/Import/pycxx_5_2_1
export PYTHON=/usr/bin/python${PY_MAJ}.${PY_MIN}
export PYDIR=python${PY_MAJ}.${PY_MIN}
export PYLIB=python${PY_MAJ}${PY_MIN}
export SVN_INC=/usr/include/subversion-1
export SVN_LIB=/usr/lib
export APR_INC=/usr/include/apr-0
export APR_LIB=/usr/lib

# always build rapidsvn into an unversioned dir
export SVNCPP_INC=${HOME}/subversion/Linux_rh90/rapidsvn/include
export SVNCPP_LIB=${HOME}/subversion/Linux_rh90/rapidsvn/src/svncpp/.libs
export LD_LIBRARY_PATH=${SVNCPP_LIB}

# prove the python version selected is as expected
${PYTHON} -c "import sys;print 'Info: Python Version',sys.version"
