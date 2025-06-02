# Create a static library from our source files 
add_library(bomber_core_lib STATIC
    graphics.c
    physics.c
)

# Tell bomber_core_lib where to find include dirs 
target_include_directories(bomber_core_lib
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${PS2_INCLUDE_DRS}
)

# Link bomber_core_lib against the PS2 libraries it needs
target_link_libraries(bomber_core_lib
    PRIVATE
    gskit
    gskit_toolkit
    dmakit
    draw
    math3d
)