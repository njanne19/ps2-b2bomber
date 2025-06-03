cmake_minimum_required(VERSION 3.0) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(ps2_flightcomputer)

# Define two boolean options: 
# - BUILD_EE : whether to build the EE-side executable (flightcomputer.elf) 
# - BUILD_IOP : whether to build the IOP-side modules (.irx) 
option(BUILD_EE "Build the EE-side executable" ON)
option(BUILD_IOP "Build the IOP-side modules" OFF)

# Check that the environment variables are set. 
if(NOT DEFINED ENV{PS2SDK})
    message(FATAL_ERROR "PS2SDK environment variable is not set. Please set it to the path of your PS2SDK installation.")
elseif(NOT DEFINED ENV{PS2DEV})
    message(FATAL_ERROR "PS2DEV environment variable is not set. Please set it to the path of your PS2DEV installation.")
endif()

# Cache those for subdirectories to reference:
set(PS2SDK "$ENV{PS2SDK}" CACHE INTERNAL "PS2SDK root")
set(PS2DEV "$ENV{PS2DEV}" CACHE INTERNAL "PS2DEV root")

# Require a toolchain file: 
if(NOT CMAKE_TOOLCHAIN_FILE)
  message(FATAL_ERROR
    "You must supply -DCMAKE_TOOLCHAIN_FILE=<path>:\n"
    "  for EE builds:   \$PS2SDK/ps2dev.cmake\n"
    "  for IOP builds:  \$PS2SDK/ps2dev_iop.cmake\n"
    "Examples:\n"
    "  cmake . -DBUILD_EE=ON  -DCMAKE_TOOLCHAIN_FILE=\${PS2SDK}/ps2dev.cmake\n"
    "  cmake . -DBUILD_IOP=ON -DCMAKE_TOOLCHAIN_FILE=\${PS2SDK}/ps2dev_iop.cmake\n"
  )
endif()


# Configure where the artifacts go 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/ee") 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/iop")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Conditionally add subdirectories 
if (BUILD_EE)
    message(STATUS "Building EE-side executable...")
    add_subdirectory(ee)
endif()

if (BUILD_IOP)
    message(STATUS "Building IOP-side modules...")
    add_subdirectory(iop)
endif()