function(embed_irx TARGET NAME IRX_PATH) 
# Where to drop generated sources
    set(C_FILE "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_irx.c")
    set(O_FILE "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_irx.o")

    # ❶ Generate .c with bin2c  (only reruns when IRX changes)
    add_custom_command(
        OUTPUT  ${C_FILE}
        COMMAND ${PS2SDK}/bin/bin2c ${IRX_PATH} ${C_FILE} ${NAME}_irx
        DEPENDS ${IRX_PATH})

    # ❷ Compile the .c with the normal PS2 EE compiler
    add_custom_command(
        OUTPUT  ${O_FILE}
        COMMAND ${CMAKE_C_COMPILER} -c ${C_FILE} -o ${O_FILE}
                $<TARGET_PROPERTY:${TARGET},COMPILE_OPTIONS>
        DEPENDS ${C_FILE})

    # ❸ Attach object to the requesting target
    target_sources(${TARGET} PRIVATE ${O_FILE})
endfunction()