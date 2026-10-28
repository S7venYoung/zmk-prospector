if(CONFIG_ZMK_PROSPECTOR_SCANNER_ORIGINAL)
  set(ms2_src ${ZEPHYR_PROSPECTOR_ZMK_MODULE_MODULE_DIR}/boards/shields/src)
  if(NOT EXISTS "${ms2_src}/s7789_update.h")
    message(FATAL_ERROR "Original ms2 themes require kbladewht/prospector-zmk-module master (37146e4 or compatible)")
  endif()
  # Retain ms2's board, panel, backlight, data bridge, NVS, and log page.
  # Its stock manager has all three original renderer calls commented out.
  set_source_files_properties(
      ${ms2_src}/prospector_layouts.c ${ms2_src}/field_layout.c
      ${ms2_src}/operator_layout.c ${ms2_src}/radii_layout.c
      TARGET_DIRECTORY app PROPERTIES HEADER_FILE_ONLY ON)
  target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${ms2_src})
  target_sources(app PRIVATE
      src/prospector_layouts.c src/field_layout.c
      src/operator_layout.c src/radii_layout.c
      src/yads_layout.c src/yads_history.c src/yads_input.c src/industrial_layout.c src/industrial_numeric_32.c src/industrial_battery_32.c src/industrial_display_56.c src/industrial_brand_28.c
      ${CMAKE_CURRENT_SOURCE_DIR}/fonts/impact_16.c
      ${CMAKE_CURRENT_SOURCE_DIR}/fonts/impact_20.c)
  target_sources_ifdef(CONFIG_ZMK_PROSPECTOR_DYA_SETTINGS app PRIVATE src/dya_settings.c)
endif()

if(CONFIG_ZMK_PROSPECTOR_DYA_SETTINGS)
  target_sources(app PRIVATE src/dya_battery_bridge.c)
endif()
