if(CONFIG_ZMK_PROSPECTOR_THEME_WEATHER_CLOCK)
  target_include_directories(app PRIVATE
      ${ZEPHYR_PROSPECTOR_ZMK_MODULE_MODULE_DIR}/boards/shields/prospector_adapter/src
      ${ZEPHYR_PROSPECTOR_ZMK_MODULE_MODULE_DIR}/boards/shields/prospector_adapter/include
      ${CMAKE_CURRENT_SOURCE_DIR}/../../host-status/include)
  target_sources(app PRIVATE src/theme.c)
  zephyr_link_libraries(-Wl,--allow-multiple-definition)
endif()
