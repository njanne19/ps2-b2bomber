cmake_minimum_required(VERSION 3.0) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(ps2-flightcomputer C) 

# Tell CMake to use the PS2 toolchain file: 
# We assume PS2SDK and PS2DEV are exported in the shell 
set(CMAKE_TOOLCHAIN_FILE "${PS2SDK}/ps2dev.cmake" CACHE STRING "")

# Then list out the include directories of the PS2 SDK and DEV libraries: 
set(PS2_INCLUDE_DRS
   ${PS2DEV}/gskit/include
   ${PS2DEV}/dmakit/include
   ${PS2SDK}/ee/include
   ${PS2SDK}/common/include
)

# As well as the libraries that we build 
set (PROJECT_INCLUDE_DIRS
   ${CMAKE_SOURCE_DIR}/include
)

# Add our source code subdirectory 
add_subdirectory(src)

# And define the executable target: 
add_executable(flightcomputer.elf 
   src/main.c
)

# Tell CMake where to find the include directories 
target_include_directories(flightcomputer.elf
   PRIVATE
   ${PS2_INCLUDE_DRS}
   ${PROJECT_INCLUDE_DIRS}
)

# Link against our core libraries (built in src/) plus whatever PS2 libraries we need.
target_link_libraries(flightcomputer.elf PRIVATE
   # My core libraries
   bomber_core_lib
   
   # PS2 libraries
   gskit
   gskit_toolkit
   dmakit
   draw
   math3d
)