#!/bin/bash

# This script runs the various Multovl executables
# from within a Docker container.
# Specify this script as the ENTRYPOINT.
# 2024-12-15 András Aszódi

COMMANDS="anctrack.awk | bed2gff.sh | config_info | multovl | multovlprob | multovltest.sh | parmultovlprob"

# the Multovl version is set as an env var in the Dockerfile
TAG=${MULTOVL_VER}-$(uname -m)

# -- Functions --

print_help() {
    cat <<HELP
Usage: docker run --rm --name multovl aaszodi/multovl:${TAG} <command> [<args>]
    where <command> is one of:
    ${COMMANDS}
    and <args> are optional arguments to the selected <command>
Don't forget to (bind-)mount a directory containing the input and output files
HELP
}

# == MAIN ==

if [[ $# -lt 1 ]]; then
    print_help
    exit 1
fi
cmd=$1
if [[ "$COMMANDS" =~ "$cmd" ]]; then
    exec $@
else
    echo "Cannot run command $cmd"
    print_help
    exit 2
fi
