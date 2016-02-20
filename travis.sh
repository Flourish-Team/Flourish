#!/usr/bin/env sh

echo "In build script"
cd Premake
chmod +x ./bin/linux/premake5
./bin/linux/premake5 make
cd ../Projects/make
make
make test