add_subdirectory(display-themes)

if(CONFIG_ZMK_KEY_STATS)
  target_include_directories(app PRIVATE include)
  target_sources(app PRIVATE src/key_stats.c)
endif()
add_subdirectory(modules/zmk-feature-runtime-macro)

target_sources_ifdef(CONFIG_PROSPECTOR_DYA_BOOTLOADER_ACTION app PRIVATE src/dya_bootloader_action.c)
