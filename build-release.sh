cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=$PS2SDK/ps2dev.cmake \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build