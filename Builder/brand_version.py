import sys
import os
import re

version_details = sys.argv[1]
input_filename = sys.argv[2]
output_filename = input_filename[:-len('.template')]

# create dictionary of branding strings
branding_info = {}

for line in file( version_details ):
	line = line.strip()
	if len(line) == 0:
		continue
	if line[0:1] == ['#']:
		continue

	key, value = [s.strip() for s in line.split('=',1)]
	branding_info[ key ] = value

build_revision = os.popen( 'svnversion -c "%s" 2>&1' % os.environ.get( 'PYSVN_EXPORTED_FROM', '..' ), 'r' ).read().strip()
# build_revision is either a range nnn:mmm or mmm
# we only want the mmm
build_revision = build_revision.split(':')[-1]
print 'Info: revision %s' % build_revision

revision, modifiers = re.compile( '(\d+)(.*)' ).search( build_revision ).groups()

if modifiers:
	branding_info['BUILD'] = '0'
else:
	branding_info['BUILD'] = revision


# read all the input text
text = file( input_filename, 'r' ).read()
# and write of a branded version
file( output_filename, 'w' ).write( text % branding_info )

sys.exit(0)
