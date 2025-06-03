rm -rf build
mkdir -p build
cmake -B build \
  -DBUILD_EE=ON \
  -DBUILD_IOP=OFF \
  -DCMAKE_TOOLCHAIN_FILE=$PS2SDK/ps2dev.cmake

cmake --build build --target all