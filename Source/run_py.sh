#!/bin/sh
export PYTHONPATH=${WORKDIR}/Source
export LD_LIBRARY_PATH=${SVNCPP_LIB}
${PYTHON} $*
