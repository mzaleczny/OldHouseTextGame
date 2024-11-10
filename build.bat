rmdir /s /q .\build
mkdir build
mkdir build\install
cd build
cmake ..
cmake --build . --config Release --verbose
cd ..
