#!/bin/bash
PWIZ_EXAMPLE_ROOT=$(pwd)
pushd "$PWIZ_EXAMPLE_ROOT" > /dev/null


#check to make sure the script is being run from the directory it resides in
if [ ! -e "$PWIZ_EXAMPLE_ROOT/clean.sh" ]; then
	echo "clean.sh must be run from the directory it resides in!"
	exit 1
fi

echo "Cleaning Pwiz Example..."

#clean boost
echo "cleaning boost extract"
find pwiz_libraries -type d -depth -regex 'pwiz_libraries/boost_[0-9]_[0-9]+_[0-9]' -exec rm -r {} \; ;
echo "cleaning gd extract"
find pwiz_libraries -type d -depth -regex 'pwiz_libraries/gd-[0-9]\.[0-9]\.[0-9]+' -exec rm -r {} \; ;
echo "cleaning zlib extract"
find pwiz_libraries -type d -depth -regex 'pwiz_libraries/zlib-[0-9]\.[0-9]\.[0-9]' -exec rm -r {} \; ;
echo "cleaning boost ext"
if [ -d ext ]; then rm -r ext ; fi; 
echo "cleaning boost build"
if [ -d pwiz_libraries/boost-build/jam_src/bin ]; then rm -r pwiz_libraries/boost-build/jam_src/bin; fi;
if [ -d pwiz_libraries/boost-build/jam_src/bootstrap ]; then rm -r pwiz_libraries/boost-build/jam_src/bootstrap; fi;
echo "cleaning proteowizard source extracted"
if [ -d pwiz_src ]; then rm -r pwiz_src ; fi;
echo "cleaning Pwiz Example source binary"
if [ -e bin/pwiz_example ]; then rm bin/pwiz_example ; fi;
echo "cleaning Pwiz Example folder"
if [ -d project_src/bin ]; then rm -r project_src/bin ; fi;
