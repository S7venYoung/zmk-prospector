/* Copyright (c) 2026 S7venYoung; SPDX-License-Identifier: MIT */
#include "segment_layout.h"
#include "fonts_carrefinho.h"
#include "connection_icon.h"
#include <stdio.h>
#include <string.h>
LV_FONT_DECLARE(segment_18);
LV_FONT_DECLARE(segment_26);
LV_FONT_DECLARE(segment_36);
LV_FONT_DECLARE(segment_48);
LV_FONT_DECLARE(segment_80);
#define AMBER 0xFFB91D
#define DARK 0x060706
#define DIM 0x443719
#define OFF 0x65635A
static lv_obj_t *status_icon;
static lv_obj_t *root,*layer,*speed,*output,*numbers[2],*cells[2][14],*mods[4];
static bool ready,last_valid[2];
static uint8_t last_speed,last_output,last_levels[2],last_mods;
static char layer_buf[32],speed_buf[4],output_buf[4],number_buf[2][4];
static lv_obj_t *box(lv_obj_t *p,int x,int y,int w,int h,uint32_t color,int radius) {
    lv_obj_t *o=lv_obj_create(p);lv_obj_remove_style_all(o);
    lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,h);
    lv_obj_set_style_bg_opa(o,LV_OPA_COVER,0);lv_obj_set_style_bg_color(o,lv_color_hex(color),0);
    lv_obj_set_style_radius(o,radius,0);lv_obj_remove_flag(o,LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_CLICKABLE);return o;
}
static lv_obj_t *label(lv_obj_t *p,int x,int y,int w,const lv_font_t *font,const char *s) {
    lv_obj_t *o=lv_label_create(p);lv_obj_remove_style_all(o);
    lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,font->line_height);
    lv_obj_set_style_text_font(o,font,0);lv_obj_set_style_text_color(o,lv_color_hex(AMBER),0);
    lv_label_set_long_mode(o,LV_LABEL_LONG_DOT);lv_label_set_text(o,s);return o;
}
lv_obj_t *segment_layout_create(lv_obj_t *parent) {
    ready=false;layer_buf[0]=0;
    root=box(parent,0,0,280,240,DARK,0);
    label(root,22,18,100,&segment_18,"CORNE");box(root,20,41,240,1,AMBER,0);
    box(root,192,15,1,20,AMBER,0);
    lv_obj_t *badge=box(root,201,15,59,23,DARK,4);
    lv_obj_set_style_border_color(badge,lv_color_hex(AMBER),0);lv_obj_set_style_border_width(badge,1,0);
    status_icon=connection_icon_create(badge,2,1,AMBER);
    output=label(badge,21,3,36,&segment_18,"USB");
    label(root,20,58,92,&segment_18,"LAYER");
    layer=label(root,20,82,94,&segment_36,"BASE");
    box(root,116,53,1,78,AMBER,0);
    speed=label(root,135,49,86,&segment_80,"42");
    lv_obj_set_style_text_align(speed,LV_TEXT_ALIGN_LEFT,0);
    label(root,222,113,38,&segment_18,"WPM");
    for(int hand=0;hand<2;hand++) {
        int y=143+hand*32;
        lv_obj_t *bar=box(root,20,y,202,22,DARK,8);
        lv_obj_set_style_border_color(bar,lv_color_hex(AMBER),0);lv_obj_set_style_border_width(bar,1,0);
        for(int i=0;i<14;i++)cells[hand][i]=box(bar,4+i*14,4,12,14,DIM,0);
        numbers[hand]=label(root,230,y-2,32,&segment_26,"--");
    }
    box(root,20,202,240,1,AMBER,0);
    const char *symbols[]={SYMBOL_COMMAND,SYMBOL_OPTION,SYMBOL_CONTROL,SYMBOL_SHIFT};
    for(int i=0;i<4;i++) {
        mods[i]=label(root,37+i*56,204,38,&Symbols_Semibold_32,symbols[i]);
        lv_obj_set_style_text_align(mods[i],LV_TEXT_ALIGN_CENTER,0);
        if(i<3)box(root,83+i*56,209,1,23,DIM,0);
    }
    struct prospector_keyboard_data empty={0};segment_layout_update(&empty,"BASE");return root;
}
void segment_layout_update(const struct prospector_keyboard_data *data,const char *name) {
    if(!root||!data)return;
    char upper[32];size_t n=0;if(!name||!*name)name="BASE";
    for(;name[n]&&n<sizeof(upper)-1;n++)upper[n]=name[n]>='a'&&name[n]<='z'?name[n]-32:name[n];
    upper[n]=0;
    if(!strcmp(upper,"DEFAULT LAYER")||!strcmp(upper,"BASE LAYER"))strcpy(upper,"BASE");
    if(!strcmp(upper,"LOWER LAYER"))strcpy(upper,"LOWER");
    if(!strcmp(upper,"RAISE LAYER"))strcpy(upper,"RAISE");
    if(strcmp(upper,layer_buf)) {
        strcpy(layer_buf,upper);
        const lv_font_t *font=strcmp(upper,"BASE")?&segment_18:&segment_36;
        lv_obj_set_style_text_font(layer,font,0);lv_obj_set_height(layer,font->line_height);
        lv_obj_set_y(layer,82+(36-font->line_height)/2);lv_label_set_text(layer,layer_buf);
    }
    if(!ready||last_speed!=data->wpm_value) {
        last_speed=data->wpm_value;snprintf(speed_buf,sizeof(speed_buf),"%u",last_speed);
        const lv_font_t *font=last_speed>=100?&segment_48:&segment_80;
        lv_obj_set_style_text_font(speed,font,0);lv_obj_set_height(speed,font->line_height);
        lv_obj_set_width(speed,last_speed>=100?75:86);
        lv_obj_set_y(speed,49+(80-font->line_height)/2);lv_label_set_text_static(speed,speed_buf);
    }
    connection_icon_update(status_icon,data->usb_connected,data->ble_connected);
    uint8_t out=data->usb_connected?2:data->ble_connected?1:0;
    if(!ready||out!=last_output) {last_output=out;snprintf(output_buf,sizeof(output_buf),"%s",out==2?"USB":out?"BLE":"OFF");lv_label_set_text_static(output,output_buf);}
    uint8_t levels[]={data->battery_level,data->peripheral_battery[0]};
    for(int hand=0;hand<2;hand++) {
        bool valid=data->has_dynamic_data&&levels[hand]>0&&levels[hand]<=100;
        if(ready&&valid==last_valid[hand]&&(!valid||levels[hand]==last_levels[hand]))continue;
        last_valid[hand]=valid;last_levels[hand]=levels[hand];
        if(valid)snprintf(number_buf[hand],sizeof(number_buf[hand]),"%u",levels[hand]);else strcpy(number_buf[hand],"--");
        /* 100 uses compact glyphs to stay inside the number column. */
        const lv_font_t *font=valid&&levels[hand]==100?&segment_18:&segment_26;
        lv_obj_set_style_text_font(numbers[hand],font,0);lv_obj_set_height(numbers[hand],font->line_height);
        lv_obj_set_y(numbers[hand],141+hand*32+(26-font->line_height)/2);
        lv_label_set_text_static(numbers[hand],number_buf[hand]);
        for(int i=0;i<14;i++)lv_obj_set_style_bg_color(cells[hand][i],lv_color_hex(valid&&levels[hand]*14>i*100?AMBER:DIM),0);
    }
    const uint8_t masks[]={0x88,0x44,0x11,0x22};
    for(int i=0;i<4;i++)if(!ready||!!(last_mods&masks[i])!=!!(data->modifier_flags&masks[i]))
        lv_obj_set_style_text_color(mods[i],lv_color_hex(data->modifier_flags&masks[i]?AMBER:OFF),0);
    last_mods=data->modifier_flags;ready=true;
}
void segment_layout_destroy(void) {if(root)lv_obj_delete(root);root=NULL;ready=false;}
