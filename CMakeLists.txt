if(CONFIG_ZMK_KEY_STATS)
  target_include_directories(app PRIVATE include)
endif()

if(CONFIG_ZMK_KEY_STATS)
  target_sources(app PRIVATE src/key_stats.c)
endif()

add_subdirectory(modules/zmk-feature-runtime-macro)

if(CONFIG_SHIELD_PROSPECTOR_ADAPTER)
  # Redirect ZMK's status-screen entry point to the selected user theme while
  # retaining the Prospector module's live Dongle widgets and services.
  zephyr_link_options(-Wl,--wrap=zmk_display_status_screen)
  target_include_directories(app PRIVATE
      ${ZEPHYR_PROSPECTOR_ZMK_MODULE_MODULE_DIR}/boards/shields/prospector_adapter/src)
  target_sources(app PRIVATE src/themes/prospector_theme_screen.c)
endif()
