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
case $(uname -s) in
Darwin)
	export PYTHON=/usr/bin/python${PY_MAJ}.${PY_MIN}
	;;
FreeBSD)
	export PYTHON=/usr/local/bin/python${PY_MAJ}.${PY_MIN}
	;;

*)
	export PYTHON=/usr/bin/python${PY_MAJ}.${PY_MIN}
	;;
esac

# prove the python version selected is as expected
${PYTHON} -c "import sys;print 'Info: Python Version %r' % sys.version"
