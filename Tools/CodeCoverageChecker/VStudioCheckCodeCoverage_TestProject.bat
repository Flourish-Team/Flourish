@echo off

set REL_PATH=..\..\
set ABS_PATH=

rem // Save current directory and change to target directory
pushd %REL_PATH%

rem // Save value of CD variable (current directory)
set ABS_PATH=%CD%

rem // Restore original directory
popd

"%ABS_PATH%\3rdParty\OpenCppCoverage\OpenCppCoverage.exe" --sources %ABS_PATH%\Libs\ -- %ABS_PATH%\Tools\Bin\Debug\UnitTestRunner.exe