import sys

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

# read all the input text
text = file( input_filename, 'r' ).read()
# and write of a branded version
file( output_filename, 'w' ).write( text % branding_info )

sys.exit(0)
