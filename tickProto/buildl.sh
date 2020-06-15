mkdir -p build
pushd build
cmake ../CMakeLists.txt && make install
popd
