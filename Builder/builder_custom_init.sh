#!/bin/echo Usage: . $0
export WORKDIR=$(cd ..;pwd)

export PY_MAJ=2
export PY_MIN=3
export SVNVER=0.37.0
export PYCXX=${WORKDIR}/Import/pycxx_5_2_1
export PYTHON=/usr/bin/python${PY_MAJ}.${PY_MIN}
export PYDIR=python${PY_MAJ}.${PY_MIN}
export PYLIB=python${PY_MAJ}${PY_MIN}
export SVN_INC=/usr/include/subversion-1
export SVN_LIB=/usr/lib
export APR_INC=/usr/include/apr-0
export APR_LIB=/usr/lib
export SVNCPP_INC=${HOME}/subversion/Linux_rh90/rapidsvn/include
export SVNCPP_LIB=${HOME}/subversion/Linux_rh90/rapidsvn/src/svncpp/.libs

# prove the python version selected is as expected
${PYTHON} -c "import sys;print 'Info: Python Version',sys.version"
