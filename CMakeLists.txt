if(CONFIG_ZMK_KEY_STATS)
  target_include_directories(app PRIVATE include)
endif()

if(CONFIG_ZMK_KEY_STATS)
  target_sources(app PRIVATE src/key_stats.c)
endif()

add_subdirectory(modules/zmk-feature-runtime-macro)

if(CONFIG_SHIELD_PROSPECTOR_ADAPTER)
  # The Prospector module also supplies this hook. Keep the application theme's
  # definition (linked first) and print the providers in the Action log so the
  # selected implementation can be audited.
  zephyr_link_libraries(-Wl,--allow-multiple-definition
                        -Wl,--trace-symbol=zmk_display_status_screen)
  target_include_directories(app PRIVATE
      ${ZEPHYR_PROSPECTOR_ZMK_MODULE_MODULE_DIR}/boards/shields/prospector_adapter/src)
  target_sources(app PRIVATE src/themes/prospector_theme_screen.c)
endif()
