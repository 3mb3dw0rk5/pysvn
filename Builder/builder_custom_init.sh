#!/bin/echo Usage: . $0

# TARGET is set by the ReleaseEngineering scripts
if [ -z "$TARGET" ]
then
	# set the ReleaseEngineering symbols in development mode
	case $(uname -s) in
	Darwin)
		. ../../ReleaseEngineering/MacOSX/software-version.inc
		;;
	*)
		. ../../ReleaseEngineering/unix/software-version.inc\
		;;
	esac
fi
export WORKDIR=$(cd ..;pwd)

# default to 2.3 if no value in $1 and $2
export PY_MAJ=${1:-2}
export PY_MIN=${2:-3}
export PYCXX=${WORKDIR}/Import/pycxx_${PYCXXVER}
export PYDIR=python${PY_MAJ}.${PY_MIN}
export PYLIB=python${PY_MAJ}${PY_MIN}
case $(uname -s) in
Darwin)
	export PYTHON=/usr/bin/python${PY_MAJ}.${PY_MIN}
	export SVN_INC=/sw/include/subversion-1
	export SVN_LIB=/sw/lib
	export APR_INC=/sw/include/apr-0
	export APR_LIB=/sw/lib
	;;

*)
	export PYTHON=/usr/bin/python${PY_MAJ}.${PY_MIN}
	export SVN_INC=/usr/include/subversion-1
	export SVN_LIB=/usr/lib
	if [ -e /usr/include/apr-0 ]
	then
		export APR_INC=/usr/include/apr-0
	else
		export APR_INC=/usr/include/apache2
	fi
	export APR_LIB=/usr/lib
	;;
esac

# prove the python version selected is as expected
${PYTHON} -c "import sys;print 'Info: Python Version %r' % sys.version"
