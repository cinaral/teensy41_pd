#!/bin/bash

#* project's relative path with respect to this script
PROJECT_PATH="../.."

shared_dir="shared"
teensy_dir="teensy"
rasppi_dir="rasppi"

echo "Usage: copy_to_rasppi.sh <TargetFolderPath (Example: usr@192.168.1.1:~/project)>"

#* change the cwd to the script dir temporarily, and hide pushd popd output
pushd () { 
	command pushd "$@" > /dev/null 
}
popd () { 
	command popd "$@" > /dev/null 
}
pushd "$(dirname ${BASH_SOURCE:0})"
trap popd EXIT #*

#* cd to project path
pushd $PROJECT_PATH

if [ "$#" -lt 1 ]; then
	echo "No target folder path provided!"
else
	echo "Syncing to $1..."
	rsync -aR \
		$shared_dir/include/config.hpp \
		$shared_dir/README.md \
		$teensy_dir/.pio/build/teensy41/firmware.hex \
		$teensy_dir/README.md \
		$teensy_dir/scripts/teensy_loader_cli/ \
		$rasppi_dir/scripts/ \
		$rasppi_dir/include/ \
		$rasppi_dir/src/ \
		$rasppi_dir/test/ \
		$rasppi_dir/CMakeLists.txt/ \
		$rasppi_dir/README.md \
		$1/
fi
popd

echo "Done."
