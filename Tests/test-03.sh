#!/bin/sh
echo WorkDir: ${WORKDIR}
if [ -e testroot-03 ]
then
    rm -rf testroot-03
fi
mkdir testroot-03
cd testroot-03

mkdir configdir
python ../test_callbacks.py
