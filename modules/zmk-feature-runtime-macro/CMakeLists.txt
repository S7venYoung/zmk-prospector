zephyr_include_directories(include)
if(CONFIG_ZMK_RUNTIME_MACRO)
    target_sources(app PRIVATE
        src/runtime_macro.c
        src/runtime_macro_dt_defaults.c
        src/behaviors/behavior_runtime_macro.c
    )

    if(CONFIG_ZMK_RUNTIME_MACRO_TEST)
        target_sources(app PRIVATE src/test/runtime_macro_test.c)
    endif()

    if(CONFIG_ZMK_RUNTIME_MACRO_DT_DEFAULTS_TEST)
        target_sources(app PRIVATE src/test/runtime_macro_dt_defaults_test.c)
    endif()

    if(CONFIG_ZMK_RUNTIME_MACRO_STUDIO_RPC)
        target_sources(app PRIVATE src/studio/runtime_macro_handler.c)

        list(APPEND CMAKE_MODULE_PATH ${ZEPHYR_BASE}/modules/nanopb)
        include(nanopb)
        set(NANOPB_GENERATE_CPP_APPEND_PATH TRUE)
        set(NANOPB_GENERATE_CPP_STANDALONE OFF)

        # NOTE: adding to app target directly causes build issues, so we create a library instead
        zephyr_library()
        file(GLOB_RECURSE PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto)

        nanopb_generate_cpp(proto_srcs proto_hdrs RELPATH ${CMAKE_CURRENT_SOURCE_DIR} ${PROTO_FILES})
        target_include_directories(${ZEPHYR_CURRENT_LIBRARY} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
        target_sources(${ZEPHYR_CURRENT_LIBRARY} PRIVATE ${proto_srcs} ${proto_hdrs})

        # app should depend on the generated proto files
        target_include_directories(app PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/proto)
        add_dependencies(app ${ZEPHYR_CURRENT_LIBRARY})
    endif()
endif()
