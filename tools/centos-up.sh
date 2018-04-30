#!/bin/sh
PRL_ROOT=/Applications/Parallels\ Desktop.app/Contents/MacOS
if [ -z ${1} ]; then
    "${PRL_ROOT}/launcher" start
    "${PRL_ROOT}/prlctl" start CentOS\ 6 
    echo Maybe:"${PRL_ROOT}/prlctl" set CentOS\ 6 --on-window-close keep-running
else
    "${PRL_ROOT}/prlctl" stop CentOS\ 6 
    "${PRL_ROOT}/launcher" stop
fi
echo ssh -f -X VM-IP gnome-terminal
