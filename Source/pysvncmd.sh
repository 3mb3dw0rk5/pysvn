#!/bin/bash -x
case "$(uname -s)" in
Darwin)
    export LD_LIBRARY_PATH=/usr/local/svn-1.8.15/lib:/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr/lib
    ;;
*)
    ;;
esac

export PYTHONPATH=${BUILDER_TOP_DIR}/Source:${BUILDER_TOP_DIR}/Examples/Client
${PYTHON} ${BUILDER_TOP_DIR}/Examples/Client/svn_cmd.py "$@"
