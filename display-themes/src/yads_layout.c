/* Copyright (c) 2026 S7venYoung
 * SPDX-License-Identifier: MIT
 * 280x240 adaptation of the 4.1-display-themes monochrome YADS information UI.
 */
#include "yads_layout.h"
#include "yads_input.h"
#include "fonts_carrefinho.h"
#include <stdio.h>
#include <string.h>
#define YADS_BACKGROUND lv_color_black()
#define YADS_FOREGROUND lv_color_white()
static lv_obj_t *root, *wpm, *output, *typed, *mods, *battery[2], *fill[2];
static lv_timer_t *idle_timer;
static struct prospector_keyboard_data state;
static uint8_t previous_modifiers;
static bool modifiers_ready;
static void plain(lv_obj_t *o) {
    lv_obj_remove_style_all(o); lv_obj_set_style_bg_color(o,YADS_BACKGROUND,0);
    lv_obj_set_style_bg_opa(o,LV_OPA_COVER,0);
    lv_obj_remove_flag(o,LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_CLICKABLE);
}
static lv_obj_t *label(lv_obj_t *p,int x,int y,int width,const lv_font_t *font) {
    lv_obj_t *o=lv_label_create(p);lv_label_set_text(o,"");lv_obj_set_style_text_color(o,YADS_FOREGROUND,0);
    lv_obj_set_style_text_font(o,font,0);lv_obj_set_style_text_align(o,LV_TEXT_ALIGN_CENTER,0);
    lv_obj_set_pos(o,x,y);lv_obj_set_width(o,width);lv_label_set_long_mode(o,LV_LABEL_LONG_DOT);
    return o;
}
static void history_tick(lv_timer_t *timer) {
    (void)timer;if(!typed)return;
    char name[9],text[32];memcpy(name,state.current_layer_name,8);name[8]='\0';
    yads_input_text(state.active_layer,name,text);lv_label_set_text(typed,text);
}
lv_obj_t *yads_layout_create(lv_obj_t *parent) {
    root=lv_obj_create(parent);plain(root);lv_obj_set_size(root,280,240);
    wpm=label(root,18,18,112,&FG_Medium_20);
    output=label(root,150,18,112,&FG_Medium_20);
    lv_obj_set_style_text_align(wpm,LV_TEXT_ALIGN_LEFT,0);
    lv_obj_set_style_text_align(output,LV_TEXT_ALIGN_RIGHT,0);
    typed=label(root,18,86,244,&FR_Regular_36);
    lv_obj_set_height(typed,FR_Regular_36.line_height);
    mods=lv_obj_create(root);plain(mods);lv_obj_set_pos(mods,32,135);lv_obj_set_size(mods,216,40);
    for(int i=0;i<2;i++) {
        int x=i?168:18;
        battery[i]=label(root,x,178,94,&FR_Regular_30);
        lv_obj_t *track=lv_obj_create(root);plain(track);lv_obj_set_pos(track,x,211);lv_obj_set_size(track,94,6);
        lv_obj_set_style_border_width(track,1,0);lv_obj_set_style_border_color(track,YADS_FOREGROUND,0);
        fill[i]=lv_obj_create(track);plain(fill[i]);lv_obj_set_style_bg_color(fill[i],YADS_FOREGROUND,0);
        lv_obj_set_size(fill[i],92,4);lv_obj_set_pos(fill[i],0,0);
    }
    memset(&state,0,sizeof(state));modifiers_ready=false;yads_input_enable(true);
    idle_timer=lv_timer_create(history_tick,100,NULL);
    yads_layout_update(&state);
    return root;
}
void yads_layout_update(const struct prospector_keyboard_data *data) {
    if (!root || !data) return;
    state = *data;
    lv_label_set_text_fmt(wpm,"%u WPM",state.wpm_value);
    if(state.usb_connected)lv_label_set_text(output,"USB");
    else lv_label_set_text_fmt(output,state.ble_connected?"BLE %u":"BLE %u X",state.profile_slot+1);
    history_tick(NULL);
    /* Active-only modifiers expand symmetrically, each with its underline. */
    const uint8_t masks[]={0x88,0x44,0x11,0x22};
    const char *symbols[]={SYMBOL_COMMAND,SYMBOL_OPTION,SYMBOL_CONTROL,SYMBOL_SHIFT};
    if (!modifiers_ready || previous_modifiers != state.modifier_flags) {
    lv_obj_clean(mods);int count=0;for(int i=0;i<4;i++)if(state.modifier_flags&masks[i])count++;
    int pos=(216-(count ? count*44-4 : 0))/2;
    for(int i=0;i<4;i++)if(state.modifier_flags&masks[i]) {
        lv_obj_t *symbol=label(mods,pos,0,40,&Symbols_Semibold_32);lv_label_set_text(symbol,symbols[i]);
        lv_obj_t *line=lv_obj_create(mods);plain(line);lv_obj_set_style_bg_color(line,YADS_FOREGROUND,0);
        lv_obj_set_pos(line,pos+2,35);lv_obj_set_size(line,36,2);pos+=44;
    }
    previous_modifiers=state.modifier_flags;modifiers_ready=true;
    }
    uint8_t values[]={state.battery_level,state.peripheral_battery[0]};
    for(int i=0;i<2;i++) {
        bool valid=state.has_dynamic_data&&values[i]>0&&values[i]<=100;
        if(valid)lv_label_set_text_fmt(battery[i],"%u",values[i]);else lv_label_set_text(battery[i],"X");
        int width=valid?(values[i]*92+99)/100:1;if(width<1)width=1;
        lv_obj_set_width(fill[i],width);lv_obj_set_x(fill[i],i?92-width:0);
    }
}
void yads_layout_destroy(void) {
    if(idle_timer){lv_timer_delete(idle_timer);idle_timer=NULL;}
    yads_input_enable(false);if(root)lv_obj_delete(root);
    root=wpm=output=typed=mods=NULL;battery[0]=battery[1]=fill[0]=fill[1]=NULL;
}
