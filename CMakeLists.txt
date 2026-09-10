if(CONFIG_ZMK_KEY_STATS OR CONFIG_ZMK_CUSTOM_SETTINGS)
  target_include_directories(app PRIVATE include)
endif()

if(CONFIG_ZMK_KEY_STATS)
  target_sources(app PRIVATE src/key_stats.c)
endif()

if(CONFIG_ZMK_CUSTOM_SETTINGS)
  target_sources(app PRIVATE src/display_settings.c)
endif()
