if(CONFIG_ZMK_PROSPECTOR_THEME_WALLE)
  target_include_directories(app PRIVATE
      ${ZEPHYR_PROSPECTOR_ZMK_MODULE_MODULE_DIR}/boards/shields/prospector_adapter/src
      ${ZEPHYR_PROSPECTOR_ZMK_MODULE_MODULE_DIR}/boards/shields/prospector_adapter/include)
  target_sources(app PRIVATE src/walle_theme.c)
  zephyr_link_libraries(-Wl,--allow-multiple-definition)
endif()
