#!/usr/bin/env sh

echo "Building Premake"
cd Premake/source/build/gmake.unix
make
echo "Building Flourish"
cd ../../../
chmod +x ./source/bin/premake5
./source/bin/premake5 make
cd ../Projects/make
make
make test