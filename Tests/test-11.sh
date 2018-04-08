#!/bin/bash
#
#   test-11.sh - 1.1- commands
#       test list --search
#

# need to get rid of any symbolic links in the WORKDIR
export WORKDIR=$( ${PYTHON} -c 'import os;os.chdir("..");print( os.getcwd() )' )

cd ${WORKDIR}/Tests
echo WorkDir: ${WORKDIR}
echo PYTHON: ${PYTHON}
echo Username: $(id -u -n)

cmd () {
    echo Info: CWD: $(pwd)
    echo Info: Command: $*
    "$@"
}

cmd_pysvn () {
    echo Info: CWD: $(pwd)
    echo Info: pysvn command: $*
    ${PYSVN} "$@"
}

cmd mkdir testroot-11
cmd cd testroot-11

TESTROOT=${WORKDIR}/Tests/testroot-11

cmd mkdir tmp
export TMPDIR=${TESTROOT}/tmp

export PYTHONPATH=${WORKDIR}/Source:${WORKDIR}/Examples/Client
export PYSVN="${PYTHON} ${WORKDIR}/Examples/Client/svn_cmd.py --pysvn-testing 01.10.00 --config-dir ${TESTROOT}/configdir"
echo Info: PYSVN command ${PYSVN}

cmd svnadmin create ${TESTROOT}/repos

echo Info: Setup - mkdir
cmd_pysvn mkdir file://${TESTROOT}/repos/trunk -m "test-11 add trunk"

echo Info: Setup - checkout wc1
cmd_pysvn checkout file://${TESTROOT}/repos/trunk ${TESTROOT}/wc1
cmd cd ${TESTROOT}/wc1

echo Info: Setup - add files and folders

cmd_pysvn mkdir folder1
cmd_pysvn mkdir folder2

echo test add file 1 >folder1/file-a.txt
cmd_pysvn add folder1/file-a.txt

echo test add file 2 >folder2/file-a.sh
cmd_pysvn add folder2/file-a.sh

echo test add file 3 >folder2/file-a.cmd
cmd_pysvn add folder2/file-a.cmd

cmd_pysvn checkin -m "commit added files"

echo test list no patterns
cmd_pysvn list --recursive

echo test list 1 pattern
cmd_pysvn list --recursive --search '*.txt'

echo test list 2 patterns
cmd_pysvn list --recursive --search '*.sh' --search '*.txt'

true
