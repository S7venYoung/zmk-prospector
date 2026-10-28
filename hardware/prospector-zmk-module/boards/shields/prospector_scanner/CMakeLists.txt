# Prospector Scanner CMakeLists.txt
# Step-by-step restoration from display_test


set(PROSPECTOR_SHIELD_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if(CONFIG_SHIELD_PROSPECTOR_SCANNER)
    # Backlight initialization (always needed)
    target_sources(app PRIVATE ${PROSPECTOR_SHIELD_SRC}/backlight_init.c)

    # Crash recovery: turn fatal errors into a logged reboot instead of a
    # silent halt that looks like a display freeze
    target_sources(app PRIVATE ${PROSPECTOR_SHIELD_SRC}/fault_recovery.c)

    # CUSTOM screen support (from display_test)
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE ${PROSPECTOR_SHIELD_SRC}/custom_status_screen.c)
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE ${PROSPECTOR_SHIELD_SRC}/hello_widget.c)

    # 状态屏数据源（替代原模块 scanner_core.c，接口名为 ble_*）
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE ${PROSPECTOR_SHIELD_SRC}/s7789_update.c)

    # 电量（本机电量 + 左右手缓存，app/src/battery_cb.c 通过 ble_battery_update() 推送）
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE ${PROSPECTOR_SHIELD_SRC}/s7789_update_battery.c)

    # Swipe gesture event (ZMK event system - thread-safe)
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE ${PROSPECTOR_SHIELD_SRC}/events/swipe_gesture_event.c)

    # Keyboard state tracking lives in the module (src/scanner_core.c) so that
    # every scanner shield shares it - nothing shield-local to add here.

    # Display settings persistence (NVS)
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE ${PROSPECTOR_SHIELD_SRC}/display_settings.c)

    # 层名仓库（可改 + NVS 持久化）：YADS2 层滚筒与广播短字段都从这里取名字
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE ${PROSPECTOR_SHIELD_SRC}/layer_names.c)

    # Brightness control with APDS9960 sensor for auto brightness
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE ${PROSPECTOR_SHIELD_SRC}/brightness_control.c)

    # System settings widget (Bootloader, Reset, Channel selector)
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE ${PROSPECTOR_SHIELD_SRC}/system_settings_widget.c)

    # NerdFont for modifier icons
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE ${PROSPECTOR_SHIELD_SRC}/fonts/NerdFonts_Regular_40.c)

    # Prospector Display layouts (Carrefinho-inspired)
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE ${PROSPECTOR_SHIELD_SRC}/prospector_layouts.c)
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE ${PROSPECTOR_SHIELD_SRC}/field_layout.c)
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE ${PROSPECTOR_SHIELD_SRC}/operator_layout.c)
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE ${PROSPECTOR_SHIELD_SRC}/radii_layout.c)
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE ${PROSPECTOR_SHIELD_SRC}/yads2_layout.c)

    # Carrefinho custom fonts
    target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/DINish_Condensed_Semibold_20.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/DINish_Condensed_Semibold_22.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/DINish_Expanded_Light_36.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/DINish_Medium_24.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/FG_Medium_20.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/FG_Medium_21.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/FG_Medium_24.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/FG_Medium_26.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/FR_Medium_32.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/FR_Regular_30.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/FR_Regular_36.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/FR_Regular_48.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/FR_Thin_48.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/PPF_NarrowThin_64.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/Symbols_Bold_26.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/Symbols_Medium_28.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/Symbols_Regular_28.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/Symbols_Semibold_28.c
        ${PROSPECTOR_SHIELD_SRC}/fonts_carrefinho/Symbols_Semibold_32.c
    )

    # Include path for the shared sources (headers live next to the .c files)
    target_include_directories(app PRIVATE ${PROSPECTOR_SHIELD_SRC})
endif()

# ORIGINAL FULL BUILD - TO BE RESTORED INCREMENTALLY
# if(CONFIG_SHIELD_PROSPECTOR_SCANNER)
#   zephyr_library()
#   ... (original code preserved in comments)
# endif()
