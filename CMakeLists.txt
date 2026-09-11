if(CONFIG_ZMK_KEY_STATS)
  target_include_directories(app PRIVATE include)
endif()

if(CONFIG_ZMK_KEY_STATS)
  target_sources(app PRIVATE src/key_stats.c)
endif()

add_subdirectory(modules/zmk-feature-runtime-macro)

if(CONFIG_SHIELD_PROSPECTOR_ADAPTER)
  # Zephyr 4.1 has no zephyr_link_options(); this supported helper places
  # --wrap on the final firmware link through zephyr_interface.
  zephyr_link_libraries(-Wl,--wrap=zmk_display_status_screen)
  target_include_directories(app PRIVATE
      ${ZEPHYR_PROSPECTOR_ZMK_MODULE_MODULE_DIR}/boards/shields/prospector_adapter/src)
  target_sources(app PRIVATE src/themes/prospector_theme_screen.c)
endif()
