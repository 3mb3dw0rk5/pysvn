#!/bin/sh
# need to get rid of any symbolic links in the WORKDIR
cd ${WORKDIR}
export WORKDIR=$( $PYTHON -c 'import os;print os.getcwd()' )

cd ${WORKDIR}/Tests
echo WorkDir: ${WORKDIR}
echo PYTHON: ${PYTHON}
echo Username: $(id -u -n)

cmd () {
	echo Info: Command: $*
	"$@"
}

cmd mkdir testroot-01
cmd cd testroot-01

TESTROOT=${WORKDIR}/Tests/testroot-01

cmd mkdir tmp
export TMPDIR=${TESTROOT}/tmp

export PYTHONPATH=${WORKDIR}/Source:${WORKDIR}/Examples/Client
export PYSVN="${PYTHON} ${WORKDIR}/Examples/Client/svn_cmd.py --config-dir ${TESTROOT}/configdir"

cmd svnadmin create ${TESTROOT}/repos

echo Info: Testing - mkdir
cmd ${PYSVN} mkdir file://${TESTROOT}/repos/trunk -m "test-01 add trunk"
cmd ${PYSVN} mkdir file://${TESTROOT}/repos/trunk/test -m "test-01 add test"

echo Info: Testing - ls
cmd ${PYSVN} ls file://${TESTROOT}/repos -v -R

echo Info: Testing - checkout
cmd ${PYSVN} checkout file://${TESTROOT}/repos/trunk ${TESTROOT}/wc1
cmd python ${WORKDIR}/Tests/find.py ${TESTROOT}/wc1
cmd cd ${TESTROOT}/wc1/test

echo Info: Testing - add
echo test add file 1 >file1.txt
echo test add file 2 >file2.txt
echo test add file 3 >file3.txt
echo test add file 4 >file4.txt
echo test add --force file 5 >file5.txt
cmd ${PYSVN} add file1.txt
cmd ${PYSVN} add file2.txt
cmd ${PYSVN} add file3.txt
cmd ${PYSVN} add file4.txt
cmd ${PYSVN} add file5.txt
cmd ${PYSVN} checkin -m "commit added files"

echo Info: Testing - update - get a new wc that will update
cmd ${PYSVN} checkout file://${TESTROOT}/repos/trunk ${TESTROOT}/wc2

echo Info: Testing - - checkin a mod from wc1
echo line 2 >>${TESTROOT}/wc1/test/file1.txt
cmd ${PYSVN} checkin -m "commit modified file"

echo Info: Testing - update
cmd ${PYSVN} update ${TESTROOT}/wc2

echo Info: Testing - the rest in lexical order

echo Info: Testing - annotate
cmd ${PYSVN} annotate ${TESTROOT}/wc2/test/file1.txt

echo Info: Testing - cat
cmd ${PYSVN} cat -r head file://${TESTROOT}/repos/trunk/test/file1.txt

echo Info: Testing - cleanup

echo Info: Testing - copy
cmd ${PYSVN} mkdir file://${TESTROOT}/repos/tags -m "test-01 add tags"
echo tag the trunk >msg.tmp
cmd ${PYSVN} copy file://${TESTROOT}/repos/trunk file://${TESTROOT}/repos/tags/version1 <msg.tmp
rm msg.tmp
cmd ${PYSVN} ls -v file://${TESTROOT}/repos/tags
cmd ${PYSVN} copy ${TESTROOT}/wc2/test/file1.txt ${TESTROOT}/wc2/test/file1b.txt
cmd ${PYSVN} propset svn:eol-style native ${TESTROOT}/wc2/test/file1b.txt
cmd ${PYSVN} checkin ${TESTROOT}/wc2 -m "copy test"

echo Info: Testing - diff
echo new line >>${TESTROOT}/wc2/test/file1b.txt
cmd ${PYSVN} diff ${TESTROOT}/wc2

echo Info: Testing - export
cmd ${PYSVN} export file://${TESTROOT}/repos/trunk/test ${TESTROOT}/export1.native
cmd ${PYSVN} export --native-eol CR file://${TESTROOT}/repos/trunk/test ${TESTROOT}/export1.cr
cmd ${PYSVN} export --native-eol LF file://${TESTROOT}/repos/trunk/test ${TESTROOT}/export1.lf
cmd ${PYSVN} export --native-eol CRLF file://${TESTROOT}/repos/trunk/test ${TESTROOT}/export1.crlf
python ${WORKDIR}/Tests/find.py ${TESTROOT}/export1.native
python ${WORKDIR}/Tests/find.py ${TESTROOT}/export1.cr
python ${WORKDIR}/Tests/find.py ${TESTROOT}/export1.lf
python ${WORKDIR}/Tests/find.py ${TESTROOT}/export1.crlf

echo Info: Testing - import

echo Info: Testing - info
cmd ${PYSVN} info ${TESTROOT}/wc2/test
cmd ${PYSVN} info ${TESTROOT}/wc2/test/file1.txt

echo Info: Testing - log
cmd ${PYSVN} log ${TESTROOT}/wc2

echo Info: Testing - ls
cmd ${PYSVN} ls file://${TESTROOT}/repos/trunk/test
cmd ${PYSVN} ls -v file://${TESTROOT}/repos/trunk/test
cmd ${PYSVN} ls ${TESTROOT}/wc2/test
cmd ${PYSVN} ls -v ${TESTROOT}/wc2/test

echo Info: Testing - merge

echo Info: Testing - mkdir - done above

echo Info: Testing - move
echo move url test >msg.tmp
cmd ${PYSVN} move file://${TESTROOT}/repos/trunk/test/file2.txt file://${TESTROOT}/repos/trunk/test/file2b.txt <msg.tmp
rm msg.tmp
cmd ${PYSVN} move ${TESTROOT}/wc2/test/file3.txt ${TESTROOT}/wc2/test/file3b.txt
cmd ${PYSVN} checkin ${TESTROOT}/wc2 -m "move wc test"

echo Info: Testing - status
echo file 4 is changing >>${TESTROOT}/wc1/test/file4.txt
cmd ${PYSVN} checkin ${TESTROOT}/wc1 -m "change wc1 for status -u to detect"

cmd ${PYSVN} status ${TESTROOT}/wc2
cmd ${PYSVN} status --verbose ${TESTROOT}/wc2
cmd ${PYSVN} status --show-updates ${TESTROOT}/wc2
cmd ${PYSVN} status --show-updates --verbose ${TESTROOT}/wc2
cmd ${PYSVN} update
cmd ${PYSVN} status --show-updates ${TESTROOT}/wc2
cmd ${PYSVN} status --show-updates --verbose ${TESTROOT}/wc2
cmd ${PYSVN} checkin ${TESTROOT}/wc2 -m "prop change"

echo Info: Testing - propdel
cd ${TESTROOT}/wc2/test
cmd ${PYSVN} propset test:prop1 del_me file4.txt
cmd ${PYSVN} proplist -v file4.txt
cmd ${PYSVN} propdel test:prop1 file4.txt
cmd ${PYSVN} proplist -v file4.txt

echo Info: Testing - propget
cmd ${PYSVN} propset svn:eol-style native file4.txt
cmd ${PYSVN} propget svn:eol-style file4.txt
cmd ${PYSVN} propget unknown file4.txt

echo Info: Testing - proplist - see above

echo Info: Testing - propset
cd ${TESTROOT}/wc2/test
cmd ${PYSVN} proplist -v file4.txt
cmd ${PYSVN} propset svn:eol-style native file4.txt
cmd ${PYSVN} proplist -v file4.txt

echo Info: Testing - remove
cd ${TESTROOT}/wc2/test
cmd ${PYSVN} remove file5.txt
cmd ${PYSVN} status

echo Info: Testing - resolved
echo conflict in file4 yes >>${TESTROOT}/wc1/test/file4.txt
echo conflict in file4 no >>${TESTROOT}/wc2/test/file4.txt
cmd ${PYSVN} checkin ${TESTROOT}/wc1/test -m "make a conflict part 1"
cmd ${PYSVN} update ${TESTROOT}/wc2/test
cmd ${PYSVN} status
cmd cp ${TESTROOT}/wc2/test/file4.txt.mine ${TESTROOT}/wc2/test/file4.txt
cmd ${PYSVN} resolved ${TESTROOT}/wc2/test/file4.txt
cmd ${PYSVN} checkin ${TESTROOT}/wc2/test/file4.txt -m "resolve a confict part 2"

echo Info: Testing - revert
cmd ${PYSVN} revert file5.txt
cmd ${PYSVN} status

echo Info: Testing - revproplist
cmd ${PYSVN} revproplist file://${TESTROOT}/repos/trunk

echo Info: Testing - revpropget
cmd ${PYSVN} revpropget svn:log file://${TESTROOT}/repos/trunk
cmd ${PYSVN} revpropget no_such_prop file://${TESTROOT}/repos/trunk

echo Info: Testing - revpropset
cmd ${PYSVN} revpropset svn:log "Hello world" file://${TESTROOT}/repos/trunk

echo Info: Testing - revpropdel
cmd ${PYSVN} revpropdel svn:log file://${TESTROOT}/repos/trunk

echo Info: Testing - status - see above

echo Info: Testing - relocate
cmd mkdir ${TESTROOT}/root
cmd mv ${TESTROOT}/repos ${TESTROOT}/root
cmd ${PYSVN} info ${TESTROOT}/wc1
cmd ${PYSVN} relocate file://${TESTROOT}/repos/trunk file://${TESTROOT}/root/repos/trunk ${TESTROOT}/wc1
cmd ${PYSVN} info ${TESTROOT}/wc1
cmd ${PYSVN} info ${TESTROOT}/wc2
cmd ${PYSVN} relocate file://${TESTROOT}/repos/trunk file://${TESTROOT}/root/repos/trunk ${TESTROOT}/wc2
cmd ${PYSVN} info ${TESTROOT}/wc2

echo Info: Testing - switch
cmd ${PYSVN} info ${TESTROOT}/wc2
cmd ${PYSVN} switch ${TESTROOT}/wc2 file://${TESTROOT}/root/repos/tags/version1
cmd ${PYSVN} info ${TESTROOT}/wc2

echo Info: Testing - update - see above
