@call "%VS170COMNTOOLS%VsDevCmd.bat"
mkdir build-msvc
cd build-msvc
cmake ..
cmake --build -j . --target install