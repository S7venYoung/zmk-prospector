/* SPDX-License-Identifier: MIT
 * Native 280x240 cassette deck, drafting plate and phosphor terminal.
 * Static mechanics/schematic are decorative; only actual received data lights.
 */
#include "concept_layouts.h"
#include "fonts_carrefinho.h"
#include "connection_icon.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
LV_FONT_DECLARE(impact_16);
LV_FONT_DECLARE(impact_20);
LV_FONT_DECLARE(industrial_battery_32);
LV_FONT_DECLARE(industrial_numeric_32);
LV_FONT_DECLARE(blueprint_title_40);
LV_FONT_DECLARE(blueprint_numeric_32);
LV_FONT_DECLARE(terminal_title_52);
LV_IMAGE_DECLARE(tape_cassette_image);

static lv_obj_t *root, *art, *title, *speed, *output, *icon, *numbers[2], *mods[4];
static lv_obj_t *vu, *history_obj, *battery_bar[2], *footer;
static lv_obj_t *tape_keys[4], *tape_lamps[4];
static lv_timer_t *history_timer;
static prospector_layout_t kind;
static uint32_t ink, paper, accent, dim;
static bool ready, valid[2], live;
static uint8_t levels[2], last_wpm, last_mods, last_out;
static char footer_text[40], layer_name[32], speed_text[4], output_text[4], number_text[2][4];
static uint8_t history[60], history_count, history_head;

static lv_obj_t *panel(lv_obj_t *parent,int x,int y,int w,int h,uint32_t color,int radius) {
    lv_obj_t *o=lv_obj_create(parent);lv_obj_remove_style_all(o);
    lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,h);
    lv_obj_set_style_bg_color(o,lv_color_hex(color),0);lv_obj_set_style_bg_opa(o,LV_OPA_COVER,0);
    lv_obj_set_style_radius(o,radius,0);lv_obj_remove_flag(o,LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_CLICKABLE);return o;
}
static lv_obj_t *label(int x,int y,int w,const lv_font_t *font,uint32_t color,const char *text) {
    lv_obj_t *o=lv_label_create(root);lv_obj_remove_style_all(o);
    lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,font->line_height);
    lv_obj_set_style_text_font(o,font,0);lv_obj_set_style_text_color(o,lv_color_hex(color),0);
    lv_label_set_long_mode(o,LV_LABEL_LONG_DOT);lv_label_set_text(o,text);return o;
}
static void rect(lv_layer_t *l,int x,int y,int w,int h,uint32_t color,bool fill,int radius) {
    lv_draw_rect_dsc_t d;lv_draw_rect_dsc_init(&d);
    d.bg_color=lv_color_hex(color);d.bg_opa=fill?LV_OPA_COVER:LV_OPA_TRANSP;
    d.border_color=lv_color_hex(color);d.border_opa=LV_OPA_COVER;d.border_width=fill?0:1;d.radius=radius;
    lv_area_t a={x,y,x+w-1,y+h-1};lv_draw_rect(l,&d,&a);
}
static void line(lv_layer_t *l,int x,int y,int x2,int y2,uint32_t color,int width) {
    lv_draw_line_dsc_t d;lv_draw_line_dsc_init(&d);d.color=lv_color_hex(color);d.width=width;d.opa=LV_OPA_COVER;
    d.p1=(lv_point_precise_t){x,y};d.p2=(lv_point_precise_t){x2,y2};lv_draw_line(l,&d);
}
static void circle(lv_layer_t *l,int x,int y,int radius,uint32_t color,int width) {
    lv_draw_arc_dsc_t d;lv_draw_arc_dsc_init(&d);d.center=(lv_point_t){x,y};
    d.radius=radius;d.start_angle=0;d.end_angle=360;d.color=lv_color_hex(color);d.width=width;d.opa=LV_OPA_COVER;lv_draw_arc(l,&d);
}
static lv_obj_t *drawing(int x,int y,int w,int h,lv_event_cb_t cb) {
    lv_obj_t *o=lv_obj_create(root);lv_obj_remove_style_all(o);lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,h);
    lv_obj_remove_flag(o,LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);lv_obj_add_flag(o,LV_OBJ_FLAG_USER_1);
    lv_obj_add_event_cb(o,cb,LV_EVENT_DRAW_MAIN,NULL);return o;
}
static void tape_player_draw(lv_event_t *e) {
    lv_layer_t *l=lv_event_get_layer(e);
    /* One rounded brushed-metal chassis surrounds both the tape well and controls. */
    rect(l,4,4,272,232,0x454A3D,true,38);
    rect(l,5,5,270,230,0xBBB49A,false,37);
    rect(l,8,8,264,224,0x222820,true,34);
    for(int y=12;y<229;y+=3) {
        int inset=y<30?38:y>213?33:12;
        line(l,inset,y,279-inset,y,0x2A3027,1);
    }
    rect(l,8,33,264,119,0x0B100C,true,12);
    rect(l,8,33,264,119,0x777B63,false,12);
    line(l,20,154,260,154,0x10150F,2);
    line(l,20,156,260,156,0x626650,1);
    rect(l,19,159,91,39,0x10170F,true,4);
    rect(l,19,159,91,39,0x6B7255,false,4);
    line(l,118,176,251,176,0x73775C,1);
    line(l,20,199,260,199,0x11160F,2);
    for(int side=0;side<2;side++) {
        int x=24+side*232;
        circle(l,x,21,3,0x9B9C89,1);line(l,x-1,20,x+1,22,0x9B9C89,1);
        circle(l,x,218,2,0x73775C,1);
    }
    for(int x=215;x<249;x+=5)line(l,x,16,x,25,0x10150F,2);
}
static void tape_draw(lv_event_t *e) {
    lv_area_t a;lv_obj_get_coords(art,&a);int x=a.x1,y=a.y1;lv_layer_t *l=lv_event_get_layer(e);
    /* Reel sectors sit on the original mechanical artwork; values remain live. */
    for(int h=0;h<2;h++) {
        int cx=x+53+h*151,cy=y+59;
        for(int k=0;k<16;k++) {
            float r=(h? -78+k*11:102+k*11)*0.01745329252f;
            uint32_t c=valid[h]&&k*100<levels[h]*16?(h?0xA5B963:accent):0x353B33;
            line(l,cx+cosf(r)*36,cy+sinf(r)*36,cx+cosf(r)*45,cy+sinf(r)*45,c,5);
        }
    }
}

static void vu_draw(lv_event_t *e) {
    lv_area_t a;lv_obj_get_coords(vu,&a);lv_layer_t *l=lv_event_get_layer(e);
    for(int k=0;k<16;k++)rect(l,a.x1+k*8,a.y1,6,6,k*180<last_wpm*16?accent:0x343B32,true,0);
}
static void blueprint_grid(lv_event_t *e) {
    lv_layer_t *l=lv_event_get_layer(e);
    for(int x=10;x<280;x+=12)line(l,x,0,x,239,x%36==10?0x163C54:0x102C40,1);
    for(int y=0;y<240;y+=12)line(l,0,y,279,y,y%36==0?0x163C54:0x102C40,1);
    line(l,22,16,165,16,paper,1);line(l,22,66,165,66,paper,1);
    line(l,26,10,17,71,paper,1);line(l,158,14,149,71,paper,1);
}
static void drafting_box(lv_layer_t *l,int cx,int cy,float angle,int px,int py,int w,int h,uint32_t color) {
    float c=cosf(angle),s=sinf(angle);
    int xx[4]={px,px+w,px+w,px}, yy[4]={py,py,py+h,py+h};
    for(int k=0;k<4;k++){int j=(k+1)%4;
        line(l,cx+xx[k]*c-yy[k]*s,cy+xx[k]*s+yy[k]*c,
             cx+xx[j]*c-yy[j]*s,cy+xx[j]*s+yy[j]*c,color,1);}
}
static void blueprint_draw(lv_event_t *e) {
    lv_area_t a;lv_obj_get_coords(art,&a);int x=a.x1,y=a.y1;lv_layer_t *l=lv_event_get_layer(e);
    for(int k=0;k<13;k++)line(l,x+k*19,y,x+k*19,y+92,0x143244,1);
    for(int k=0;k<6;k++)line(l,x,y+k*18,x+239,y+k*18,0x143244,1);
    for(int h=0;h<2;h++) {
        int cx=x+70+h*87,cy=y+43;float angle=h?-.18f:.18f;
        drafting_box(l,cx,cy,angle,-39,-27,78,53,0x70B1D3);
        for(int row=0;row<3;row++)for(int col=0;col<6;col++)
            drafting_box(l,cx,cy,angle,-36+col*12,-24+row*16+(col==0||col==5?3:0),10,13,paper);
        for(int k=0;k<3;k++)drafting_box(l,cx,cy,angle,-3+k*12,28+k*3,10,13,paper);
        for(int col=0;col<6;col++)circle(l,cx-34+col*13,cy-31,1,paper,1);
        int rx=x+13+h*177;
        line(l,rx,y+7,rx,y+83,paper,1);line(l,rx-4,y+7,rx+4,y+7,paper,1);line(l,rx-4,y+83,rx+4,y+83,paper,1);
        line(l,rx,y+7,rx-3,y+12,paper,1);line(l,rx,y+7,rx+3,y+12,paper,1);
        line(l,rx,y+83,rx-3,y+78,paper,1);line(l,rx,y+83,rx+3,y+78,paper,1);
        for(int k=0;k<8;k++)rect(l,rx-2,y+70-k*7,4,5,valid[h]&&k*100<levels[h]*8?0x76BFE2:0x223D4C,true,0);
    }
}
static void history_draw(lv_event_t *e) {
    lv_area_t a;lv_obj_get_coords(history_obj,&a);lv_layer_t *l=lv_event_get_layer(e);
    line(l,a.x1,a.y2,a.x2,a.y2,dim,1);
    unsigned ceiling=60;for(int k=0;k<history_count;k++)if(history[k]>ceiling)ceiling=history[k];
    for(int k=0;k<history_count;k++) {
        int start=history_count==60?history_head:0;
        int h=history[(start+k)%60]*32/ceiling;if(h<1)h=1;
        int x=a.x1+(60-history_count+k)*232/60;
        rect(l,x,a.y2-h,2,h,paper,true,0);
    }
}
static void sample(lv_timer_t *t) {
    (void)t;if(!history_obj||!live)return;
    bool same=history_count==60;for(int i=0;i<history_count&&same;i++)if(history[i]!=last_wpm)same=false;
    history[history_head]=last_wpm;history_head=(history_head+1)%60;if(history_count<60)history_count++;
    if(!same)lv_obj_invalidate(history_obj);
}
static void make_mods(int x,int y,int step,bool vertical) {
    const char *tags[]={"CMD","OPT","CTRL","SHIFT"};
    for(int i=0;i<4;i++)mods[i]=label(x+(vertical?0:i*step),y+(vertical?i*step:0),vertical?64:50,&lv_font_montserrat_12,dim,tags[i]);
}
static lv_obj_t *bar(int x,int y,int w,int h) {
    lv_obj_t *o=lv_bar_create(root);lv_obj_remove_style_all(o);lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,h);
    lv_obj_set_style_bg_color(o,lv_color_hex(0x203C2A),LV_PART_MAIN);lv_obj_set_style_bg_opa(o,255,LV_PART_MAIN);
    lv_obj_set_style_bg_color(o,lv_color_hex(paper),LV_PART_INDICATOR);lv_obj_set_style_bg_opa(o,255,LV_PART_INDICATOR);
    lv_bar_set_range(o,0,100);return o;
}
lv_obj_t *concept_layout_create(lv_obj_t *parent,prospector_layout_t style) {
    kind=style;ready=false;layer_name[0]=0;art=title=vu=history_obj=footer=NULL;history_timer=NULL;history_count=history_head=0;live=false;
    ink=style==PROSPECTOR_LAYOUT_TAPE?0x101510:style==PROSPECTOR_LAYOUT_BLUEPRINT?0x071D2B:0x09150F;
    paper=style==PROSPECTOR_LAYOUT_TAPE?0xD9D3B9:style==PROSPECTOR_LAYOUT_BLUEPRINT?0xB8DCEF:0x9DD98B;
    accent=style==PROSPECTOR_LAYOUT_TAPE?0xE7A245:style==PROSPECTOR_LAYOUT_BLUEPRINT?0xF0B44C:0xAEEA96;
    dim=style==PROSPECTOR_LAYOUT_TAPE?0x9B9C89:style==PROSPECTOR_LAYOUT_BLUEPRINT?0x5D8198:0x326845;
    root=panel(parent,0,0,280,240,ink,0);
    if(kind==PROSPECTOR_LAYOUT_BLUEPRINT)lv_obj_add_event_cb(root,blueprint_grid,LV_EVENT_DRAW_MAIN,NULL);
    if(kind==PROSPECTOR_LAYOUT_TAPE)lv_obj_add_event_cb(root,tape_player_draw,LV_EVENT_DRAW_MAIN,NULL);
    icon=output=NULL;
    if(kind!=PROSPECTOR_LAYOUT_TAPE) {
        label(24,17,103,&impact_16,paper,"CORNE");
        icon=connection_icon_create(root,205,15,paper);output=label(229,18,31,&impact_16,paper,"OFF");
    }
    if(kind==PROSPECTOR_LAYOUT_TAPE) {
        label(48,12,158,&lv_font_montserrat_12,paper,"STEREO / DATA DECK");
        lv_obj_t *cassette=lv_image_create(root);lv_image_set_src(cassette,&tape_cassette_image);lv_obj_set_pos(cassette,12,38);
        art=drawing(12,38,256,108,tape_draw);
        title=label(84,46,112,&FG_Medium_20,0x080A08,"BASE");
        lv_obj_set_style_text_align(title,LV_TEXT_ALIGN_CENTER,0);
        for(int h=0;h<2;h++){numbers[h]=label(44+h*151,81,42,&industrial_battery_32,paper,"--");lv_obj_set_style_text_align(numbers[h],LV_TEXT_ALIGN_CENTER,0);}
        speed=label(27,160,76,&industrial_numeric_32,paper,"0");lv_obj_set_style_text_align(speed,LV_TEXT_ALIGN_RIGHT,0);
        label(118,158,43,&impact_16,paper,"WPM");
        vu=drawing(118,182,128,6,vu_draw);
        const char *tags[]={"CMD","OPT","CTRL","SHIFT"};
        for(int i=0;i<4;i++) {
            tape_keys[i]=panel(root,30+i*55,203,50,25,0x161D15,3);
            lv_obj_set_style_border_color(tape_keys[i],lv_color_hex(0x71765D),0);
            lv_obj_set_style_border_width(tape_keys[i],1,0);
            tape_lamps[i]=panel(root,36+i*55,206,6,3,dim,1);
            mods[i]=label(33+i*55,212,44,&impact_16,dim,tags[i]);
            lv_obj_set_style_text_align(mods[i],LV_TEXT_ALIGN_CENTER,0);
        }
    } else if(kind==PROSPECTOR_LAYOUT_BLUEPRINT) {
        /* Header is a drafting label, not a repeated dashboard strip. */
        lv_obj_add_flag(lv_obj_get_child(root,0),LV_OBJ_FLAG_HIDDEN);
        title=label(24,22,147,&blueprint_title_40,paper,"BASE");label(190,18,70,&impact_16,paper,"CORNE");
        panel(root,24,69,232,1,dim,0);art=drawing(20,74,240,94,blueprint_draw);
        speed=label(229,87,34,&blueprint_numeric_32,paper,"0");label(225,124,40,&lv_font_montserrat_12,paper,"WPM");
        for(int h=0;h<2;h++)numbers[h]=label(23+h*177,153,32,&impact_16,paper,"--");
        make_mods(35,171,15,true);
        panel(root,125,181,131,1,dim,0);panel(root,125,222,131,1,dim,0);
        panel(root,125,181,1,42,dim,0);panel(root,255,181,1,42,dim,0);
        label(134,185,114,&lv_font_montserrat_12,paper,"SPLIT / CORNE");footer=label(134,204,112,&lv_font_montserrat_12,dim,"LAYER: BASE");
        lv_obj_set_pos(icon,205,41);lv_obj_set_pos(output,229,43);
    } else {
        make_mods(24,44,55,false);
        panel(root,20,76,150,74,dim,0);panel(root,21,77,148,72,ink,0);
        title=label(26,82,139,&terminal_title_52,paper,"BASE");
        panel(root,179,76,1,76,dim,0);speed=label(191,77,65,&industrial_numeric_32,paper,"0");
        label(202,112,45,&lv_font_montserrat_12,paper,"WPM");
        for(int h=0;h<2;h++){battery_bar[h]=bar(190,136+h*16,28,5);numbers[h]=label(225,129+h*16,32,&lv_font_montserrat_12,paper,"--");}
        label(24,155,120,&lv_font_montserrat_12,paper,"WPM / 60s");history_obj=drawing(24,174,232,35,history_draw);
        label(25,216,42,&lv_font_montserrat_12,paper,"> _");history_timer=lv_timer_create(sample,1000,NULL);
    }
    struct prospector_keyboard_data empty={0};concept_layout_update(&empty,"BASE");return root;
}
void concept_layout_update(const struct prospector_keyboard_data *d,const char *name) {
    if(!root||!d)return;
    live=d->has_dynamic_data;
    char text[32];size_t n=0;if(!name||!*name)name="BASE";
    for(;name[n]&&n<31;n++)text[n]=name[n]>='a'&&name[n]<='z'?name[n]-32:name[n];text[n]=0;
    if(!strcmp(text,"BASE LAYER")||!strcmp(text,"DEFAULT LAYER"))strcpy(text,"BASE");
    if(strcmp(layer_name,text)) {
        strcpy(layer_name,text);
        if(footer){snprintf(footer_text,sizeof(footer_text),"LAYER: %s",text);lv_label_set_text_static(footer,footer_text);}
        if(title) {
            const lv_font_t *font=kind==PROSPECTOR_LAYOUT_TAPE?&FG_Medium_20:kind==PROSPECTOR_LAYOUT_BLUEPRINT?&blueprint_title_40:&terminal_title_52;
            lv_point_t size;lv_text_get_size(&size,text,font,0,0,LV_COORD_MAX,0);
            if(size.x>(kind==PROSPECTOR_LAYOUT_TAPE?112:kind==PROSPECTOR_LAYOUT_BLUEPRINT?147:139))font=&impact_16;
            lv_obj_set_style_text_font(title,font,0);lv_obj_set_height(title,font->line_height);lv_label_set_text(title,text);
        } else lv_obj_invalidate(art);
    }
    if(!ready||last_wpm!=d->wpm_value) {
        last_wpm=d->wpm_value;snprintf(speed_text,sizeof(speed_text),"%u",last_wpm);lv_label_set_text_static(speed,speed_text);
        if(vu)lv_obj_invalidate(vu);
    }
    uint8_t out=d->usb_connected?2:d->ble_connected?1:0;
    if(!ready||out!=last_out){last_out=out;if(output){strcpy(output_text,out==2?"USB":out?"BLE":"OFF");lv_label_set_text_static(output,output_text);}}
    if(icon)connection_icon_update(icon,d->usb_connected,d->ble_connected);
    for(int h=0;h<2;h++) {
        uint8_t value=h?d->peripheral_battery[0]:d->battery_level;
        bool ok=d->has_dynamic_data&&value>0&&value<=100;
        if(ready&&valid[h]==ok&&(!ok||levels[h]==value))continue;
        valid[h]=ok;levels[h]=value;
        if(ok)snprintf(number_text[h],4,"%u",value);else strcpy(number_text[h],"--");
        lv_label_set_text_static(numbers[h],number_text[h]);
        if(kind==PROSPECTOR_LAYOUT_TERMINAL)lv_bar_set_value(battery_bar[h],ok?value:0,LV_ANIM_OFF);
        else lv_obj_invalidate(art);
    }
    const uint8_t masks[]={0x88,0x44,0x11,0x22};
    for(int i=0;i<4;i++)if(!ready||!!(last_mods&masks[i])!=!!(d->modifier_flags&masks[i])) {
        lv_obj_set_style_text_color(mods[i],lv_color_hex(d->modifier_flags&masks[i]?accent:dim),0);
        if(kind==PROSPECTOR_LAYOUT_TAPE) {
            bool active=(d->modifier_flags&masks[i])!=0;
            lv_obj_set_style_bg_color(tape_keys[i],lv_color_hex(active?0x51442C:0x161D15),0);
            lv_obj_set_style_bg_color(tape_lamps[i],lv_color_hex(active?accent:0x404938),0);
        }
        if(kind==PROSPECTOR_LAYOUT_TERMINAL) {
            lv_obj_set_style_bg_color(mods[i],lv_color_hex(0x235530),0);
            lv_obj_set_style_bg_opa(mods[i],d->modifier_flags&masks[i]?LV_OPA_COVER:LV_OPA_TRANSP,0);
        }
    }
    last_mods=d->modifier_flags;ready=true;
}
void concept_layout_reset_history(void) {
    history_count=history_head=0;live=false;
    if(history_obj)lv_obj_invalidate(history_obj);
}
void concept_layout_destroy(void) {
    if(history_timer)lv_timer_delete(history_timer);history_timer=NULL;
    if(root)lv_obj_delete(root);root=art=title=vu=history_obj=footer=NULL;ready=false;live=false;history_count=history_head=0;
}
