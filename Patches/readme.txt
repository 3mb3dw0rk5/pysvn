This directory contains any patches that need to be applied to 3rd party code
to allow pysvn to build.

svncpp.patch
	- patch against r7023
	- add changed paths support to log

svncpp-2.patch
	- patch against r7023
	- fix order of APR init and APR use
