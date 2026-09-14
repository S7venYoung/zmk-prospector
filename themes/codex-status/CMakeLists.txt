if(CONFIG_ZMK_PROSPECTOR_THEME_CODEX_STATUS)
  target_include_directories(app PRIVATE
      ${ZEPHYR_PROSPECTOR_ZMK_MODULE_MODULE_DIR}/boards/shields/prospector_adapter/src
      ${ZEPHYR_PROSPECTOR_ZMK_MODULE_MODULE_DIR}/boards/shields/prospector_adapter/include)
  target_sources(app PRIVATE
      src/theme.c
      src/impact_16.c
      src/impact_20.c
      src/impact_48.c
      src/impact_56.c)
  target_sources(app PRIVATE src/codex_metrics.c src/codex_metrics_rpc.c)

  list(APPEND CMAKE_MODULE_PATH ${ZEPHYR_BASE}/modules/nanopb)
  include(nanopb)
  set(NANOPB_GENERATE_CPP_APPEND_PATH TRUE)
  set(NANOPB_GENERATE_CPP_STANDALONE OFF)
  zephyr_library()
  file(GLOB_RECURSE PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto)
  nanopb_generate_cpp(proto_srcs proto_hdrs RELPATH ${CMAKE_CURRENT_SOURCE_DIR} ${PROTO_FILES})
  target_include_directories(${ZEPHYR_CURRENT_LIBRARY} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
  target_sources(${ZEPHYR_CURRENT_LIBRARY} PRIVATE ${proto_srcs} ${proto_hdrs})
  target_include_directories(app PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/proto)
  target_include_directories(app PRIVATE include)
  add_dependencies(app ${ZEPHYR_CURRENT_LIBRARY})
  zephyr_link_libraries(-Wl,--allow-multiple-definition)
endif()
