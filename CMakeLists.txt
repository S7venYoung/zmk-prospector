if(CONFIG_ZMK_KEY_STATS)
  target_include_directories(app PRIVATE include)
endif()

if(CONFIG_ZMK_KEY_STATS)
  target_sources(app PRIVATE src/key_stats.c)
endif()

add_subdirectory(modules/zmk-feature-runtime-macro)

if(CONFIG_SHIELD_PROSPECTOR_ADAPTER)
  # Replace the upstream single-layout screen while retaining its live Dongle widgets.
  set(PROSPECTOR_SHIELD_CMAKE
      ${ZEPHYR_PROSPECTOR_ZMK_MODULE_MODULE_DIR}/boards/shields/prospector_adapter/CMakeLists.txt)
  file(READ ${PROSPECTOR_SHIELD_CMAKE} PROSPECTOR_SHIELD_CONTENTS)
  string(REPLACE
      "  zephyr_library_sources(src/custom_status_screen.c)"
      "  # custom_status_screen.c is supplied by the user config theme manager"
      PROSPECTOR_SHIELD_CONTENTS
      "${PROSPECTOR_SHIELD_CONTENTS}")
  file(WRITE ${PROSPECTOR_SHIELD_CMAKE} "${PROSPECTOR_SHIELD_CONTENTS}")
  target_include_directories(app PRIVATE
      ${ZEPHYR_PROSPECTOR_ZMK_MODULE_MODULE_DIR}/boards/shields/prospector_adapter/src)
  target_sources(app PRIVATE src/themes/prospector_theme_screen.c)
endif()
