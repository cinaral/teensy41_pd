#!/bin/bash

#* project relative path
PROJECT_PATH=../..
REMOVE_DIR=$PROJECT_PATH/lib/fn_PD_controller/interface

#*  change the cwd to the script dir temporarily, and hide pushd popd output
pushd () { 
	command pushd "$@" > /dev/null 
}
popd () { 
	command popd "$@" > /dev/null 
}
pushd "$(dirname ${BASH_SOURCE:0})"
trap popd EXIT #*

#* remove
if [[ -d "$REMOVE_DIR" ]]; then
rm -r $REMOVE_DIR

if [[ $? -eq 0 ]]; then
   echo "Removed $REMOVE_DIR"
fi
fi

echo "$0 done."