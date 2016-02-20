#!/usr/bin/env sh

echo "Building Premake"
cd Premake/source
make -f Bootstrap.mak $TRAVIS_OS_NAME
bin/release/premake5 test
echo "Building Flourish"
cd ../../../
chmod +x ./source/bin/premake5
./source/bin/premake5 make
cd ../Projects/make
make
make test