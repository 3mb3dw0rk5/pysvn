#!/bin/sh
set -x
echo WorkDir: ${WORKDIR}
if -e testroot-01
then
    rm -rf testroot-01
fi
mkdir testroot-01
cd testroot-01
TESTROOT=$(pwd)
export PYTHONPATH=${WORKDIR}/Source;${WORKDIR}/Examples/Client
export PYSVN="${PYTHON} ${WORKDIR}/Examples/Client/svn_cmd.py --config-dir ${TESTROOT}/configdir"

echo ${PATH}
svnadmin create ${TESTROOT}/repos

echo Info: mkdir
${PYSVN} mkdir file:///${TESTROOT}/repos/trunk -m "test-01 add trunk"
${PYSVN} mkdir file:///${TESTROOT}/repos/trunk/test -m "test-01 add test"

echo Info: ls
${PYSVN} ls file:///${TESTROOT}/repos -v -R

echo Info: checkout
${PYSVN} checkout file:///${TESTROOT}/repos/trunk ${TESTROOT}/wc1
ls -lR ${TESTROOT}/wc1
cd ${TESTROOT}/wc1/test

echo Info: add
echo test add file 1 >file1.txt
echo test add file 2 >file2.txt
echo test add file 3 >file3.txt
echo test add file 4 >file4.txt
echo test add file 5 >file5.txt
${PYSVN} add file1.txt
${PYSVN} add file2.txt
${PYSVN} add file3.txt
${PYSVN} add file4.txt
${PYSVN} add file5.txt
${PYSVN} checkin -m "commit added files"

echo Info: update - get a new wc that will update
${PYSVN} checkout file:///${TESTROOT}/repos/trunk ${TESTROOT}/wc2

echo Info: - checkin a mod from wc1
echo line 2 >>${TESTROOT}/wc1/test/file1.txt
${PYSVN} checkin -m "commit modified file"

echo Info: update
${PYSVN} update ${TESTROOT}/wc2

echo Info: the rest in lexical order

echo Info: annotate
${PYSVN} annotate ${TESTROOT}/wc2/test/file1.txt

echo Info: cat
${PYSVN} cat -r head file:///${TESTROOT}/repos/trunk/test/file1.txt

echo Info: cleanup

echo Info: copy
${PYSVN} mkdir file:///${TESTROOT}/repos/tags -m "test-01 add tags"
echo tag the trunk | ${PYSVN} copy file:///${TESTROOT}/repos/trunk file:///${TESTROOT}/repos/tags/version1
${PYSVN} ls -v file:///${TESTROOT}/repos/tags
${PYSVN} copy ${TESTROOT}/wc2/test/file1.txt ${TESTROOT}/wc2/test/file1b.txt
${PYSVN} checkin ${TESTROOT}/wc2 -m "copy test"

echo Info: diff
echo new line >>${TESTROOT}/wc2/test/file1b.txt
${PYSVN} diff ${TESTROOT}/wc2

echo Info: export
${PYSVN} export file:///${TESTROOT}/repos/trunk/test ${TESTROOT}/export1
dir /s /b ${TESTROOT}/export1

echo Info: import

echo Info: info
${PYSVN} info ${TESTROOT}/wc2/test
${PYSVN} info ${TESTROOT}/wc2/test/file1.txt

echo Info: log
${PYSVN} log ${TESTROOT}/wc2

echo Info: ls
${PYSVN} ls file:///${TESTROOT}/repos/trunk/test
${PYSVN} ls -v file:///${TESTROOT}/repos/trunk/test
${PYSVN} ls ${TESTROOT}/wc2/test
${PYSVN} ls -v ${TESTROOT}/wc2/test

echo Info: merge

echo Info: mkdir - done above

echo Info: move
echo move url test |${PYSVN} move file:///${TESTROOT}/repos/trunk/test/file2.txt file:///${TESTROOT}/repos/trunk/test/file2b.txt 
${PYSVN} move ${TESTROOT}/wc2/test/file3.txt ${TESTROOT}/wc2/test/file3b.txt
${PYSVN} checkin ${TESTROOT}/wc2 -m "move wc test"


echo Info: status
echo file 4 is changing >>${TESTROOT}/wc1/test/file4.txt
${PYSVN} checkin ${TESTROOT}/wc1 -m "change wc1 for status -u to detect"

${PYSVN} status ${TESTROOT}/wc2
${PYSVN} status --verbose ${TESTROOT}/wc2
${PYSVN} status --show-updates ${TESTROOT}/wc2
${PYSVN} status --show-updates --verbose ${TESTROOT}/wc2
${PYSVN} update
${PYSVN} status --show-updates ${TESTROOT}/wc2
${PYSVN} status --show-updates --verbose ${TESTROOT}/wc2
${PYSVN} checkin ${TESTROOT}/wc2 -m "prop change"

echo Info: propdel
cd /d ${TESTROOT}/wc2/test
${PYSVN} propset test:prop1 del_me file4.txt
${PYSVN} proplist -v file4.txt
${PYSVN} propdel test:prop1 file4.txt
${PYSVN} proplist -v file4.txt

echo Info: propget
${PYSVN} propget svn:eol-style file4.txt

echo Info: proplist - see above

echo Info: propset
cd /d ${TESTROOT}/wc2/test
${PYSVN} proplist -v file4.txt
${PYSVN} propset svn:eol-style native file4.txt
${PYSVN} proplist -v file4.txt

echo Info: remove
cd /d ${TESTROOT}/wc2/test
${PYSVN} remove file5.txt
${PYSVN} status

echo Info: resolved
echo conflict in file4 yes >>${TESTROOT}/wc1/test/file4.txt
echo conflict in file4 no >>${TESTROOT}/wc2/test/file4.txt
${PYSVN} checkin ${TESTROOT}/wc1/test -m "make a conflict part 1"
${PYSVN} update ${TESTROOT}/wc2/test
${PYSVN} status
copy ${TESTROOT}/wc2/test/file4.txt.mine ${TESTROOT}/wc2/test/file4.txt
${PYSVN} resolved ${TESTROOT}/wc2/test/file4.txt
${PYSVN} checkin ${TESTROOT}/wc2/test/file4.txt -m "resolve a confict part 2"

echo Info: revert
${PYSVN} revert file5.txt
${PYSVN} status

echo Info: revproplist
${PYSVN} revproplist file:///${TESTROOT}/repos/trunk

echo Info: revpropget
${PYSVN} revpropget svn:log file:///${TESTROOT}/repos/trunk
${PYSVN} revpropget no_such_prop file:///${TESTROOT}/repos/trunk

echo Info: revpropset
${PYSVN} revpropset svn:log "Hello world" file:///${TESTROOT}/repos/trunk

echo Info: revpropdel
${PYSVN} revpropdel svn:log file:///${TESTROOT}/repos/trunk

echo Info: status - see above

echo Info: relocate
mkdir ${TESTROOT}/root
move ${TESTROOT}/repos ${TESTROOT}/root
${PYSVN} info ${TESTROOT}/wc1
${PYSVN} relocate file:///${TESTROOT}/repos/trunk file:///${TESTROOT}/root/repos/trunk ${TESTROOT}/wc1
${PYSVN} info ${TESTROOT}/wc1
${PYSVN} info ${TESTROOT}/wc2
${PYSVN} relocate file:///${TESTROOT}/repos/trunk file:///${TESTROOT}/root/repos/trunk ${TESTROOT}/wc2
${PYSVN} info ${TESTROOT}/wc2

echo Info: switch
${PYSVN} info ${TESTROOT}/wc2
${PYSVN} switch ${TESTROOT}/wc2 file:///${TESTROOT}/root/repos/tags/version1
${PYSVN} info ${TESTROOT}/wc2

echo Info: update - see above
