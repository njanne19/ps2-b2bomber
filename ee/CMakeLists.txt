# ee/CMakeLists.txt
cmake_minimum_required(VERSION 3.0)
project(flightcomputer_ee C)


# 1) Sanity‐check that PS2SDK and PS2DEV were inherited from the top‐level:
if(NOT DEFINED PS2SDK)
  message(FATAL_ERROR "PS2SDK not defined in ee/CMakeLists.txt")
endif()
if(NOT DEFINED PS2DEV)
  message(FATAL_ERROR "PS2DEV not defined in ee/CMakeLists.txt")
endif()

# 2) PS2 SDK include paths (EE side)
set(PS2_INCLUDE_DIRS
    ${PS2DEV}/gskit/include
    ${PS2DEV}/dmakit/include
    ${PS2SDK}/ee/include
    ${PS2SDK}/common/include
)
# 3) Project-specific include paths
set(PROJECT_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/ee/include        # This resolves to <...>/ps2-flightcomputer/ee/include
)

# 4) Build the static core library from ee/src/
add_subdirectory(src)

# 5) Create the EE executable “flightcomputer.elf”
add_executable(flightcomputer.elf
    src/main.c
)

# 6) Tell the EE executable where to find headers:
#    We must include both PS2 headers and our ee/include
# message(AUTHOR_WARNING "in /ee/CMakeLists.txt PS2_INCLUDE_DIRS = ${PS2_INCLUDE_DIRS}")
# message(AUTHOR_WARNING "in /ee/CMakeLists.txt PROJECT_INCLUDE_DIRS = ${PROJECT_INCLUDE_DIRS}")
# message(AUTHOR_WARNING "in /ee/CMakeLists.txt CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
target_include_directories(flightcomputer.elf PRIVATE
    ${PS2_INCLUDE_DIRS}
    ${PROJECT_INCLUDE_DIRS}
)

# 7) Link flightcomputer.elf against bomber_core (the static lib) 
#    and any PS2 SDK libraries you need
target_link_libraries(flightcomputer.elf PRIVATE
    bomber_core    # the static library built in ee/src
    gskit
    gskit_toolkit
    dmakit
    draw
    math3d
)

# 8) Place flightcomputer.elf into build/ee/ as configured by top-level:
set_target_properties(flightcomputer.elf PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/ee"
)