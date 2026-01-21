mkdir -p build
cd build
cmake -DCCLM_BUILD_INSTALLER=ON ..
cmake --build .
mv ./cclm ../
cd ..

# g++ src/installer.cxx -o ./dist/linux/install
