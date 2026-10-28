/* Host contract test: setting/event storage is mocked, LVGL/renderers are real. */
#include <assert.h>
#include "industrial_layout.h"
#include "../../src/dya_settings.c"
void test_dya_contract(void) {
    prospector_dya_start();
    apply(NULL);assert(prospector_layouts_get_style()==PROSPECTOR_LAYOUT_FIELD);
    for(int theme=0;theme<5;theme++) {
        int before=prospector_layouts_get_style();
        display_theme_value=theme;display_palette_value=3;
        zmk_event_t event={.type=2,.changed={.setting=&display_theme}};
        changed(&event);
        assert(prospector_layouts_get_style()==before); /* event thread cannot touch LVGL */
        apply(NULL);assert(prospector_layouts_get_style()==theme+1);
        if(theme==0)assert(field_layout_get_palette()==3);
        if(theme==1)assert(operator_layout_get_palette()==3);
        if(theme==2)assert(radii_layout_get_palette()==3);
    }
    display_theme_value=99;atomic_set(&dirty,1);apply(NULL);
    assert(prospector_layouts_get_style()==PROSPECTOR_LAYOUT_INDUSTRIAL);
    assert(prospector_dya_hand_slot(0)==0&&prospector_dya_hand_slot(1)==1);
    left_hand_slot_value=1;right_hand_slot_value=0;
    assert(prospector_dya_hand_slot(0)==1&&prospector_dya_hand_slot(1)==0);
    right_hand_slot_value=1;assert(prospector_dya_hand_slot(0)==-1);
    prospector_dya_stop();
}
