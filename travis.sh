#!/usr/bin/env sh

echo "In build script"
cd Premake
bin/linux/premake5 make
cd ../Projects/make
make
make test