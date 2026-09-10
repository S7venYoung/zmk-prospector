if(CONFIG_ZMK_KEY_STATS OR CONFIG_ZMK_CUSTOM_SETTINGS)
  target_include_directories(app PRIVATE include)
endif()

if(CONFIG_ZMK_KEY_STATS)
  target_sources(app PRIVATE src/key_stats.c)
endif()

# The monochrome OLED settings must not be exposed by the Prospector receiver.
if(CONFIG_ZMK_CUSTOM_SETTINGS AND CONFIG_SHIELD_DONGLE_DISPLAY)
  target_sources(app PRIVATE src/display_settings.c)
endif()

add_subdirectory(modules/zmk-feature-runtime-macro)
