#!/usr/bin/env sh

echo "Building Premake"
echo $TRAVIS_OS_NAME
cd Premake/source
make -f Bootstrap.mak $TRAVIS_OS_NAME
bin/release/premake5 test
echo "Building Flourish"
cd ../
chmod +x ./source/bin/release/premake5
./source/bin/release/premake5 gmake
cd ../Projects/gmake
make config=$BUILD_TYPE
cd ../../

if[ "$BUILD_TYPE" = "debug" ] then
  chmod +x ./Bin/Debug/TestProject
  ./Bin/Debug/TestProject
elif [ "$BUILD_TYPE" = "release" ] then
  chmod +x ./Bin/Debug/TestProject
  ./Bin/Debug/TestProject
else
  echo "Unknown build type $BUILD_TYPE"
fi