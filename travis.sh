#!/usr/bin/env sh

echo "Building Premake"
echo $TRAVIS_OS_NAME
cd Premake/source
make -f Bootstrap.mak $TRAVIS_OS_NAME
bin/release/premake5 test
echo "Building Flourish"
cd ../
chmod +x ./source/bin/release/premake5
./source/bin/release/premake5 make
cd ../Projects/make
make
make test