#!/bin/sh
#
#Script to build pwiz_example
#This script is heavily adapted from the quickbuild.sh script originally written by Darren Kessner
#included in the ProteoWizard library (http://proteowizard.sourceforge.net)
PWIZ_EXAMPLE_ROOT=$(pwd)
if [ ! -e "$PWIZ_EXAMPLE_ROOT/quickbuild.sh" ]; then
	echo "quickbuild.sh must be run from the directory it resides in - quitting"
	exit 1
fi

PWIZ_EXAMPLE_BJAM=$PWIZ_EXAMPLE_ROOT/pwiz_libraries/boost-build/jam_src/bin/bjam

# Build local copy of bjam
if [ ! -e "$PWIZ_EXAMPLE_BJAM" ]; then
	echo "Building bjam..."
	cd "$PWIZ_EXAMPLE_ROOT/pwiz_libraries/boost-build/jam_src"
	LOCATE_TARGET=bin sh build.sh
fi

if test $(uname -s) = "Darwin"; then
	DEFAULT_TOOLSET=toolset=darwin
fi

# Build MSCalibrate
echo "Building MSCalibrate..."
cd "$PWIZ_EXAMPLE_ROOT"
export BOOST_BUILD_PATH="$PWIZ_EXAMPLE_ROOT/pwiz_libraries/boost-build"
"$PWIZ_EXAMPLE_BJAM" "$@" --libraries-path="$(pwd)/pwiz_libraries"
