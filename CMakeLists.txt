if(CONFIG_ZMK_KEY_STATS)
  target_include_directories(app PRIVATE include)
endif()

if(CONFIG_ZMK_KEY_STATS)
  target_sources(app PRIVATE src/key_stats.c)
endif()

add_subdirectory(modules/zmk-feature-runtime-macro)

if(CONFIG_SHIELD_PROSPECTOR_ADAPTER)
  # Replace the upstream single-layout screen while retaining its live Dongle widgets.
  set(PROSPECTOR_SCREEN_SOURCE
      ${ZEPHYR_PROSPECTOR_ZMK_MODULE_MODULE_DIR}/boards/shields/prospector_adapter/src/custom_status_screen.c)
  set_source_files_properties(${PROSPECTOR_SCREEN_SOURCE} PROPERTIES HEADER_FILE_ONLY ON)
  target_include_directories(app PRIVATE
      ${ZEPHYR_PROSPECTOR_ZMK_MODULE_MODULE_DIR}/boards/shields/prospector_adapter/src)
  target_sources(app PRIVATE src/themes/prospector_theme_screen.c)
endif()
