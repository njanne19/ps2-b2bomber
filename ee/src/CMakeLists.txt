# ee/src/CMakeLists.txt
cmake_minimum_required(VERSION 3.0)
project(bomber_core C)

# 1) Verify PS2SDK/PS2DEV are inherited:
if(NOT DEFINED PS2SDK)
  message(FATAL_ERROR "PS2SDK not defined in ee/src/CMakeLists.txt")
endif()
if(NOT DEFINED PS2DEV)
  message(FATAL_ERROR "PS2DEV not defined in ee/src/CMakeLists.txt")
endif()

# 2) PS2 SDK include directories (EE side)
set(PS2_INCLUDE_DIRS
    ${PS2DEV}/gskit/include
    ${PS2DEV}/dmakit/include
    ${PS2SDK}/ee/include
    ${PS2SDK}/common/include
)

# 3) Your project’s “ee/include” folder:
set(PROJECT_INCLUDE_DIRS
    "${CMAKE_SOURCE_DIR}/ee/include"   # -> .../ee/include
)

# 4) Build a static library from graphics.c and physics.c
add_library(bomber_core STATIC
    graphics.c
    physics.c
)

# 5) Tell the compiler where to find headers for this library:
#    It needs ee/include (for graphics.h, physics.h) and the PS2 SDK headers.
# message(AUTHOR_WARNING "in /ee/src/CMakeLists.txt PS2_INCLUDE_DIRS = ${PS2_INCLUDE_DIRS}")
# message(AUTHOR_WARNING "in /ee/src/CMakeLists.txt PROJECT_INCLUDE_DIRS = ${PROJECT_INCLUDE_DIRS}")
# message(AUTHOR_WARNING "in /ee/src/CMakeLists.txt CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
target_include_directories(bomber_core PRIVATE
    ${PROJECT_INCLUDE_DIRS}
    ${PS2_INCLUDE_DIRS}
)

# 6) If bomber_core uses any PS2 SDK functions, link the required lib(s) here.
#    (Often graphics/physics code will rely on GSKit or DMAKit.)
target_link_libraries(bomber_core PRIVATE
    gskit
    gskit_toolkit
    dmakit
    draw
    math3d
)

# 7) Ensure the static archive lands in build/lib/
#    (The top-level CMake set ARCHIVE_OUTPUT_DIRECTORY to ${CMAKE_BINARY_DIR}/lib.)
set_target_properties(bomber_core PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    PREFIX ""  # produce “bomber_core_lib.a” (no “lib” prefix)
)