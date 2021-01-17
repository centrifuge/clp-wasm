call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"
set BUILD_CONFIG=%1
if [%1] == [] set BUILD_CONFIG=release

set BUILD_PLATFORM=x64
set BUILD_DIR=build_%BUILD_PLATFORM%_%BUILD_CONFIG%
echo 'Running x64 build ... '
cmake -G Ninja -B%BUILD_DIR% -DCMAKE_BUILD_TYPE=%BUILD_CONFIG% .

pushd %BUILD_DIR% 
ninja
popd
