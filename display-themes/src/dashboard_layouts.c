/* Copyright (c) 2026 S7venYoung; SPDX-License-Identifier: MIT
 * ORBIT / SPLIT / MONO: state changes only, no animation or texture buffers. */
#include "dashboard_layouts.h"
#include "fonts_carrefinho.h"
#include <stdio.h>
#include <string.h>
LV_FONT_DECLARE(impact_16);
LV_FONT_DECLARE(impact_20);
LV_FONT_DECLARE(industrial_display_56);
LV_FONT_DECLARE(industrial_numeric_32);
LV_FONT_DECLARE(dashboard_numeric_52);
LV_FONT_DECLARE(dashboard_base_72);
LV_FONT_DECLARE(dashboard_mono_96);
static lv_obj_t *root,*layer,*speed,*output,*numbers[2],*meters[2],*mods[4],*index_label;
static prospector_layout_t kind;
static uint32_t accent,muted;
static bool ready,last_valid[2];
static uint8_t last_speed,last_out,last_levels[2],last_mods,last_layer;
static char name_buf[32],speed_buf[4],out_buf[4],number_buf[2][4],index_buf[4];
static lv_obj_t *box(lv_obj_t *p,int x,int y,int w,int h,uint32_t color,int radius) {
    lv_obj_t *o=lv_obj_create(p);lv_obj_remove_style_all(o);lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,h);
    lv_obj_set_style_bg_opa(o,255,0);lv_obj_set_style_bg_color(o,lv_color_hex(color),0);lv_obj_set_style_radius(o,radius,0);
    lv_obj_remove_flag(o,LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_CLICKABLE);return o;
}
static lv_obj_t *text(int x,int y,int w,const lv_font_t *f,uint32_t color,const char *s) {
    lv_obj_t *o=lv_label_create(root);lv_obj_remove_style_all(o);lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,f->line_height);
    lv_obj_set_style_text_font(o,f,0);lv_obj_set_style_text_color(o,lv_color_hex(color),0);lv_label_set_long_mode(o,LV_LABEL_LONG_DOT);
    lv_label_set_text(o,s);return o;
}
static lv_obj_t *arc(int x,int y,int size,int start,int end,uint32_t color,int width,bool battery) {
    lv_obj_t *o=lv_arc_create(root);lv_obj_remove_style_all(o);lv_obj_set_pos(o,x,y);lv_obj_set_size(o,size,size);
    lv_obj_remove_flag(o,LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_arc_width(o,width,LV_PART_MAIN);lv_obj_set_style_arc_width(o,width,LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(o,lv_color_hex(battery?0x12333F:color),LV_PART_MAIN);
    lv_obj_set_style_arc_color(o,lv_color_hex(color),LV_PART_INDICATOR);
    lv_arc_set_bg_angles(o,start,end);lv_arc_set_range(o,0,100);lv_arc_set_value(o,100);return o;
}
static lv_obj_t *bar(int x,int y,int w,int h,uint32_t track) {
    lv_obj_t *o=lv_bar_create(root);lv_obj_remove_style_all(o);lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,h);
    lv_obj_set_style_bg_opa(o,255,LV_PART_MAIN);lv_obj_set_style_bg_opa(o,255,LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(o,lv_color_hex(track),LV_PART_MAIN);lv_obj_set_style_bg_color(o,lv_color_hex(accent),LV_PART_INDICATOR);
    lv_obj_set_style_radius(o,2,LV_PART_MAIN);lv_obj_set_style_radius(o,2,LV_PART_INDICATOR);lv_bar_set_range(o,0,100);return o;
}
lv_obj_t *dashboard_layout_create(lv_obj_t *parent,prospector_layout_t style) {
    kind=style;ready=false;name_buf[0]=0;index_label=NULL;
    accent=kind==PROSPECTOR_LAYOUT_ORBIT?0x27E4F2:kind==PROSPECTOR_LAYOUT_SPLIT?0x74EAF4:0xFF913F;
    muted=kind==PROSPECTOR_LAYOUT_ORBIT?0x6A7D8B:0x747779;
    root=box(parent,0,0,280,240,kind==PROSPECTOR_LAYOUT_ORBIT?0x031421:0x131515,0);
    const uint32_t paper=0xF2EEE2;
    if(kind==PROSPECTOR_LAYOUT_ORBIT) {
        text(22,16,92,&impact_16,paper,"C O R N E");output=text(221,16,39,&impact_16,paper,"USB");
        arc(63,39,154,0,360,0x17516B,2,false);
        for(int i=0;i<4;i++)arc(69,45,142,i*90+15,i*90+60,accent,3,false);
        meters[0]=arc(8,65,112,120,240,accent,5,true);meters[1]=arc(160,65,112,300,60,accent,5,true);
        /* Mirror the left gauge: both batteries fill upward from the bottom. */
        lv_arc_set_mode(meters[1],LV_ARC_MODE_REVERSE);
        layer=text(91,67,98,&impact_20,paper,"BASE");lv_obj_set_style_text_align(layer,LV_TEXT_ALIGN_CENTER,0);
        speed=text(99,96,82,&dashboard_numeric_52,paper,"42");lv_obj_set_style_text_align(speed,LV_TEXT_ALIGN_CENTER,0);
        lv_obj_t *unit=text(115,151,50,&impact_16,paper,"WPM");lv_obj_set_style_text_align(unit,LV_TEXT_ALIGN_CENTER,0);
        numbers[0]=text(16,105,34,&impact_20,paper,"--");numbers[1]=text(234,105,30,&impact_20,paper,"--");
    } else if(kind==PROSPECTOR_LAYOUT_SPLIT) {
        box(root,185,0,95,240,0x064CEB,0);
        text(22,18,94,&impact_16,paper,"C O R N E");output=text(139,18,37,&impact_16,paper,"USB");box(root,20,43,155,1,muted,0);
        layer=text(12,78,170,&dashboard_base_72,paper,"BASE");lv_obj_set_style_text_align(layer,LV_TEXT_ALIGN_CENTER,0);
        speed=text(198,36,65,&dashboard_numeric_52,paper,"42");lv_obj_set_style_text_align(speed,LV_TEXT_ALIGN_CENTER,0);
        lv_obj_t *unit=text(210,90,43,&impact_16,paper,"WPM");lv_obj_set_style_text_align(unit,LV_TEXT_ALIGN_CENTER,0);
        box(root,201,117,60,1,0x6696F4,0);box(root,201,163,60,1,0x6696F4,0);
        for(int i=0;i<2;i++){numbers[i]=text(201,124+i*47,60,&industrial_numeric_32,paper,"--");meters[i]=bar(201,158+i*47,60,7,0x143679);}
        box(root,20,186,155,1,muted,0);
    } else {
        text(22,17,96,&impact_16,paper,"C O R N E");output=text(154,17,43,&impact_16,paper,"USB");
        box(root,232,17,1,151,muted,0);
        index_label=text(22,46,35,&impact_20,accent,"01");box(root,22,72,18,1,paper,0);
        layer=text(20,82,202,&industrial_display_56,paper,"BASE");
        speed=text(22,178,63,&industrial_numeric_32,accent,"42");text(90,188,38,&impact_16,paper,"WPM");
        box(root,139,181,1,43,muted,0);
        for(int i=0;i<2;i++){meters[i]=bar(153,186+i*18,70,8,0x3B3D3B);numbers[i]=text(231,181+i*18,32,&impact_16,paper,"--");}
    }
    const char *symbols[]={SYMBOL_COMMAND,SYMBOL_CONTROL,SYMBOL_OPTION,SYMBOL_SHIFT};
    for(int i=0;i<4;i++) {
        int x,y,w;
        if(kind==PROSPECTOR_LAYOUT_MONO){x=239;y=25+i*35;w=31;}
        else if(kind==PROSPECTOR_LAYOUT_SPLIT){x=23+i*39;y=199;w=36;}
        else{x=37+i*56;y=200;w=38;}
        mods[i]=text(x,y,w,&Symbols_Semibold_32,muted,symbols[i]);lv_obj_set_style_text_align(mods[i],LV_TEXT_ALIGN_CENTER,0);
    }
    struct prospector_keyboard_data empty={0};dashboard_layout_update(&empty,"BASE");return root;
}
void dashboard_layout_update(const struct prospector_keyboard_data *d,const char *name) {
    if(!root||!d)return;
    char upper[32];size_t n=0;if(!name||!*name)name="BASE";
    for(;name[n]&&n<sizeof(upper)-1;n++)upper[n]=name[n]>='a'&&name[n]<='z'?name[n]-32:name[n];upper[n]=0;
    if(!strcmp(upper,"DEFAULT LAYER")||!strcmp(upper,"BASE LAYER"))strcpy(upper,"BASE");
    if(!strcmp(upper,"LOWER LAYER"))strcpy(upper,"LOWER");if(!strcmp(upper,"RAISE LAYER"))strcpy(upper,"RAISE");
    if(strcmp(name_buf,upper)) {
        strcpy(name_buf,upper);const lv_font_t *font=&impact_20;
        if(kind==PROSPECTOR_LAYOUT_MONO){font=&industrial_display_56;lv_point_t s;lv_text_get_size(&s,upper,font,0,0,LV_COORD_MAX,0);if(s.x>202)font=&FR_Regular_36;lv_text_get_size(&s,upper,font,0,0,LV_COORD_MAX,0);if(s.x>202)font=&impact_20;}
        if(kind==PROSPECTOR_LAYOUT_SPLIT&&!strcmp(upper,"BASE"))font=&dashboard_base_72;
        if(kind==PROSPECTOR_LAYOUT_MONO&&!strcmp(upper,"BASE"))font=&dashboard_mono_96;
        lv_obj_set_style_text_font(layer,font,0);lv_obj_set_height(layer,font->line_height);
        lv_obj_set_y(layer,kind==PROSPECTOR_LAYOUT_ORBIT?67:kind==PROSPECTOR_LAYOUT_SPLIT?78+(74-font->line_height)/2:68+(98-font->line_height)/2);
        lv_label_set_text(layer,name_buf);
    }
    if(!ready||last_speed!=d->wpm_value) {
        last_speed=d->wpm_value;snprintf(speed_buf,sizeof(speed_buf),"%u",last_speed);
        const lv_font_t *font=kind==PROSPECTOR_LAYOUT_MONO||last_speed>=100?&industrial_numeric_32:&dashboard_numeric_52;
        lv_obj_set_style_text_font(speed,font,0);lv_obj_set_height(speed,font->line_height);
        lv_obj_set_y(speed,kind==PROSPECTOR_LAYOUT_ORBIT?96+(54-font->line_height)/2:kind==PROSPECTOR_LAYOUT_SPLIT?36+(54-font->line_height)/2:178);
        lv_label_set_text_static(speed,speed_buf);
    }
    uint8_t out=d->usb_connected?2:d->ble_connected?1:0;
    if(!ready||last_out!=out){last_out=out;snprintf(out_buf,sizeof(out_buf),"%s",out==2?"USB":out?"BLE":"OFF");lv_label_set_text_static(output,out_buf);}
    if(index_label&&(!ready||last_layer!=d->active_layer)){last_layer=d->active_layer;snprintf(index_buf,sizeof(index_buf),"%02u",(unsigned)last_layer+1);lv_label_set_text_static(index_label,index_buf);}
    uint8_t levels[]={d->battery_level,d->peripheral_battery[0]};
    for(int i=0;i<2;i++) {
        bool valid=d->has_dynamic_data&&levels[i]>0&&levels[i]<=100;
        if(ready&&last_valid[i]==valid&&(!valid||last_levels[i]==levels[i]))continue;
        last_valid[i]=valid;last_levels[i]=levels[i];
        if(valid)snprintf(number_buf[i],sizeof(number_buf[i]),"%u",levels[i]);else strcpy(number_buf[i],"--");
        if(kind==PROSPECTOR_LAYOUT_ORBIT){
            const lv_font_t *font=valid&&levels[i]==100?&impact_16:&impact_20;
            lv_obj_set_style_text_font(numbers[i],font,0);lv_obj_set_height(numbers[i],font->line_height);
        }
        lv_label_set_text(numbers[i],number_buf[i]);
        if(kind==PROSPECTOR_LAYOUT_ORBIT)lv_arc_set_value(meters[i],valid?levels[i]:0);else lv_bar_set_value(meters[i],valid?levels[i]:0,LV_ANIM_OFF);
    }
    const uint8_t masks[]={0x88,0x11,0x44,0x22};
    for(int i=0;i<4;i++)if(!ready||!!(last_mods&masks[i])!=!!(d->modifier_flags&masks[i]))lv_obj_set_style_text_color(mods[i],lv_color_hex(d->modifier_flags&masks[i]?accent:muted),0);
    last_mods=d->modifier_flags;ready=true;
}
void dashboard_layout_destroy(void){if(root)lv_obj_delete(root);root=NULL;ready=false;}
