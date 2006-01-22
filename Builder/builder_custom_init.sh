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

export PYTHON=$( which python${PY_MAJ}.${PY_MIN} )
if [ -e "${PYTHON}" ]
then
    # prove the python version selected is as expected
    ${PYTHON} -c "import sys;print 'Info: Python Version %r' % sys.version"
else
    echo "Error: Cannot find python${PY_MAJ}.${PY_MIN} on the PATH"
fi
