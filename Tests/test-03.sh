#!/bin/sh
echo WorkDir: ${WORKDIR}
echo PYTHON: ${PYTHON}

mkdir -p testroot-03
rm -rf testroot-03
mkdir testroot-03
cd testroot-03

mkdir configdir
${PYTHON} ../test_callbacks.py
