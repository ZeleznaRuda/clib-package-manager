mkdir -p build && cd build && cmake .. && cmake --build .
cd .. && g++ sources/installer.cxx -o ./dist/linux/install

