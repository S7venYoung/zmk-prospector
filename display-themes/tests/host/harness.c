#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "prospector_layouts.h"
#include "boot_animation.h"
#include "yads_history.h"
#include "yads_input.h"
#include "field_layout.h"
#include "operator_layout.h"
#include "radii_layout.h"
#include "industrial_layout.h"
#include "segment_layout.h"
#include "dashboard_layouts.h"
#include "concept_layouts.h"
#include "fonts_carrefinho.h"

uint32_t host_ticks;
uint8_t display_settings_get_max_layers(void) { return 10; }
/* The retained stock YADS2 renderer is not part of these three tests. */
lv_obj_t *yads2_layout_create(lv_obj_t *o) { return o; }
void yads2_layout_destroy(void) {}
void yads2_layout_update(uint8_t a,const char*b,uint8_t c,bool d,const uint8_t*e,
    const bool*f,uint8_t g,uint8_t h,bool i,uint8_t j,bool k,bool l,const char*m) {}

static struct yads_history input;
static bool host_long_name;
void yads_input_enable(bool enabled) { (void)enabled; yads_history_reset(&input); }
void yads_input_text(uint8_t layer,const char *name,char out[32]) {
    yads_history_text(&input,layer,host_long_name ? "VERY_LONG_LAYER_NAME" : name,host_ticks,out);
}
extern void test_dya_contract(void);
static unsigned flush_count;
static uint8_t frame[280 * 240 * 3];
static uint8_t draw_buf[280 * 32 * 3] __attribute__((aligned(64)));
static void flush(lv_display_t *d, const lv_area_t *a, uint8_t *p) {
    flush_count++;
    for (int y=a->y1;y<=a->y2;y++) for(int x=a->x1;x<=a->x2;x++) {
        /* LVGL RGB888 memory order is B,G,R. */
        unsigned src=((y-a->y1)*(a->x2-a->x1+1)+(x-a->x1))*3;
        if(x>=0 && x<280 && y>=0 && y<240) {
            unsigned dst=(y*280+x)*3;
            frame[dst]=p[src+2];frame[dst+1]=p[src+1];frame[dst+2]=p[src];
        }
    }
    lv_display_flush_ready(d);
}
static bool safe(int x,int y) {
    if(x<2||x>277||y<2||y>237) return false;
    int dx=x<43?43-x:x>236?x-236:0;
    int dy=y<43?43-y:y>196?y-196:0;
    return dx*dx+dy*dy<=41*41;
}
static int failures;
static int clipping_failures;
static int inspected;
static int overlap_failures;
static struct {lv_area_t area;const char *text;} text_regions[64];
static unsigned text_count;
static void check_overlaps(const char *theme) {
    for(unsigned i=0;i<text_count;i++)for(unsigned j=i+1;j<text_count;j++) {
        lv_area_t a=text_regions[i].area,b=text_regions[j].area;
        if(a.x1<=b.x2&&a.x2>=b.x1&&a.y1<=b.y2&&a.y2>=b.y1) {
            printf("OVERLAP %s [%s] [%s]\n",theme,text_regions[i].text,text_regions[j].text);
            overlap_failures++;
        }
    }
}
static void inspect(lv_obj_t *o,const char *theme) {
    if(lv_obj_has_flag(o,LV_OBJ_FLAG_HIDDEN))return;
    if(lv_obj_has_flag(o,LV_OBJ_FLAG_USER_1) || lv_obj_check_type(o,&lv_arc_class) || lv_obj_check_type(o,&lv_image_class) || lv_obj_check_type(o,&lv_bar_class)) {
        lv_area_t a;lv_obj_get_coords(o,&a);inspected++;
        if(!safe(a.x1,a.y1)||!safe(a.x2,a.y1)||!safe(a.x1,a.y2)||!safe(a.x2,a.y2)) {
            printf("UNSAFE %s indicator bounds=%d,%d,%d,%d\n",theme,a.x1,a.y1,a.x2,a.y2);failures++;
        }
    }
    if(lv_obj_check_type(o,&lv_label_class)) {
        const char *text=lv_label_get_text(o);
        if(text && *text && lv_obj_get_style_text_opa(o,0)>0) {
            lv_area_t a;lv_obj_get_coords(o,&a);inspected++;
            if(!strcmp(theme,"split")&&(!strcmp(text,"SYS")||!strcmp(text,"NAV"))) {
                const lv_font_t *font=lv_obj_get_style_text_font(o,0);
                lv_point_t size;lv_text_get_size(&size,text,font,0,0,LV_COORD_MAX,0);
                assert(font->line_height>=70&&size.x<=lv_obj_get_width(o));
                for(const char *c=text;*c;c++) {
                    lv_font_glyph_dsc_t glyph;assert(lv_font_get_glyph_dsc(font,&glyph,*c,0));
                    assert(glyph.box_h>40);
                }
            }

            if((!strcmp(theme,"tape")||!strcmp(theme,"blueprint")||!strcmp(theme,"terminal"))&&!strcmp(text,"WPM")) {
                lv_point_t size;lv_text_get_size(&size,text,lv_obj_get_style_text_font(o,0),0,0,LV_COORD_MAX,0);
                assert(size.x<=lv_obj_get_width(o));
            }
            if(text_count<64){text_regions[text_count].area=a;text_regions[text_count++].text=text;}
            for (lv_obj_t *p=lv_obj_get_parent(o);p;p=lv_obj_get_parent(p)) {
                if (lv_obj_has_flag(p,LV_OBJ_FLAG_OVERFLOW_VISIBLE)) continue;
                lv_area_t clip;lv_obj_get_coords(p,&clip);
                if(a.x1<clip.x1||a.y1<clip.y1||a.x2>clip.x2||a.y2>clip.y2) {
                    if(clipping_failures<1000)printf("CLIPPED %s text=%s bounds=%d,%d,%d,%d parent=%d,%d,%d,%d\n",theme,text,a.x1,a.y1,a.x2,a.y2,clip.x1,clip.y1,clip.x2,clip.y2);
                    clipping_failures++;break;
                }
            }
            if(!safe(a.x1,a.y1)||!safe(a.x2,a.y1)||!safe(a.x1,a.y2)||!safe(a.x2,a.y2)) {
                printf("UNSAFE %s text=%s bounds=%d,%d,%d,%d\n",theme,text,a.x1,a.y1,a.x2,a.y2);failures++;
            }
        }
    }
    for(uint32_t i=0;i<lv_obj_get_child_count(o);i++)inspect(lv_obj_get_child(o,i),theme);
}
static int verify_modifiers(lv_obj_t *o, uint8_t flags, int style) {
    const char *symbols[] = {style>=10?"CMD":SYMBOL_COMMAND, style>=10?"OPT":SYMBOL_OPTION, style>=10?"CTRL":SYMBOL_CONTROL, style>=10?"SHIFT":SYMBOL_SHIFT};
    const uint8_t masks[] = {0x88, 0x44, 0x11, 0x22};
    int found = 0;
    if (lv_obj_check_type(o, &lv_label_class)) {
        for (int i = 0; i < 4; i++) if (!strcmp(lv_label_get_text(o), symbols[i])) {
            bool active = (flags & masks[i]) != 0;
            if (style==5) assert(lv_color_eq(lv_obj_get_style_bg_color(lv_obj_get_parent(o), 0),
                lv_color_hex(active ? 0xFFCA13 : 0x1B2420)));
            assert(lv_color_eq(lv_obj_get_style_text_color(o, 0),
                lv_color_hex(style>=10 ? (active ? (style==10?0xE7A245:style==11?0xF0B44C:0xAEEA96) : (style==10?0x66705F:style==11?0x5D8198:0x326845)) : style>=7 ? (active ? (style==9?0xFF913F:style==8?0x74EAF4:0x27E4F2) : (style==7?0x6A7D8B:0x747779)) : style==6 ? (active ? 0xFFB91D : 0x65635A) : (active ? 0x0D1310 : 0x777C76))));
            found++;
        }
    }
    for(uint32_t i=0;i<lv_obj_get_child_count(o);i++)
        found += verify_modifiers(lv_obj_get_child(o,i), flags, style);
    return found;
}
static int verify_connection_icon(lv_obj_t *o,const char *expected) {
    int found=0;
    if(lv_obj_check_type(o,&lv_label_class)) {
        const char *s=lv_label_get_text(o);
        if(!strcmp(s,LV_SYMBOL_USB)||!strcmp(s,LV_SYMBOL_BLUETOOTH)||!strcmp(s,LV_SYMBOL_CLOSE)) {
            assert(!strcmp(s,expected));found++;
        }
    }
    for(uint32_t i=0;i<lv_obj_get_child_count(o);i++)found+=verify_connection_icon(lv_obj_get_child(o,i),expected);
    return found;
}
int main(int argc,char **argv) {
    setbuf(stdout,NULL);
    lv_init();lv_display_t *d=lv_display_create(280,240);
    lv_display_set_color_format(d,LV_COLOR_FORMAT_RGB888);
    lv_display_set_buffers(d,draw_buf,NULL,sizeof(draw_buf),LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(d,flush);
    lv_obj_t *parking_screen=lv_screen_active();
    /* Render boot frames through real LVGL, and verify one-shot cleanup. */
    unsigned top_children = lv_obj_get_child_count(lv_display_get_layer_top(d));
    prospector_layouts_init(parking_screen);
    assert(prospector_boot_active());
    unsigned boot_times[] = {0, 300, 650, 950, 1150};
    unsigned last_boot_time = 0;
    for (unsigned n = 0; n < 5; n++) {
        unsigned delta = boot_times[n] - last_boot_time;
        host_ticks += delta; lv_tick_inc(delta); lv_timer_handler();
        lv_refr_now(d);
        if (boot_times[n] < 850) {
            for (int y = 0; y < 240; y++) for (int x = 0; x < 280; x++) {
                unsigned at = (y * 280 + x) * 3;
                if (frame[at] || frame[at + 1] || frame[at + 2]) assert(safe(x, y));
            }
        }
        if (argc > 1) {
            char path[1024]; snprintf(path, sizeof(path), "%s/boot-%u.ppm", argv[1], boot_times[n]);
            FILE *f = fopen(path, "wb"); assert(f);
            fprintf(f, "P6\n280 240\n255\n"); fwrite(frame, 1, sizeof(frame), f); fclose(f);
        }
        last_boot_time = boot_times[n];
    }
    assert(!prospector_boot_active());
    assert(lv_obj_get_child_count(lv_display_get_layer_top(d)) == top_children);
    prospector_layouts_destroy();
    prospector_boot_start(parking_screen); assert(!prospector_boot_active());
    prospector_boot_stop(); prospector_boot_stop();
    /* Check forward/reverse switching while live renderers/timers exist. */
    lv_obj_t *cycle_screen=lv_obj_create(NULL);lv_obj_remove_style_all(cycle_screen);
    lv_obj_set_size(cycle_screen,280,240);prospector_layouts_init(cycle_screen);
    prospector_layouts_set_style(PROSPECTOR_LAYOUT_FIELD);
    for(int n=0;n<60;n++) {
        int expected=(n%12)+1;assert(prospector_layouts_get_style()==expected);
        prospector_layouts_next();
    }
    for(int n=0;n<60;n++) {
        prospector_layouts_prev();int expected=12-(n%12);
        assert(prospector_layouts_get_style()==expected);
    }
    prospector_layouts_destroy();lv_obj_delete(cycle_screen);
    const char *names[]={"field","operator","radii","yads","industrial","segment","orbit","split","mono","tape","blueprint","terminal"};
    for(int style=1;style<=12;style++) {
        lv_obj_t *screen=lv_obj_create(NULL);lv_obj_remove_style_all(screen);
        lv_obj_set_size(screen,280,240);lv_obj_set_style_bg_opa(screen,255,0);
        lv_obj_set_style_bg_color(screen,lv_color_black(),0);lv_screen_load(screen);
        prospector_layouts_init(screen);prospector_layouts_set_style(style);
        for(int palette=0;palette<4;palette++) {
            for(int state=0;state<6;state++) {
                struct prospector_keyboard_data data={.active_layer=state==3?9:state,
                    .wpm_value=state*40,.battery_level=state==5?100:state==4?1:state?85:0,.peripheral_battery={state==5?100:state==4?1:state==2?0:72,0,0},
                    .modifier_flags=palette==0&&state==1?0xff:(palette*6+state)%16,.profile_slot=state%5,.usb_connected=state==1,
                    .ble_connected=state==2,.ble_bonded=state>1,.has_dynamic_data=true};
                if(state==3)memcpy(data.current_layer_name,"WWWWWWWW",8);else memcpy(data.current_layer_name,"BASE",5);
                if(style>=5 && (palette & 1)) data.modifier_flags <<= 4;
                if(style>=5 && state==1) {
                    data.active_layer=0; data.wpm_value=42; data.ble_connected=true;
                    data.battery_level=83; data.peripheral_battery[0]=79; data.modifier_flags=0x0a;
                }
                host_long_name=state==3;
                prospector_layouts_update(&data);
                if(state==3) {
                    uint8_t batteries[3]={72,0,0};bool valid[3]={true,false,false};
                    if(style==1)field_layout_update(9,"VERY_LONG_LAYER_NAME",85,true,72,true,120,data.modifier_flags,false,3,false,true);
                    if(style==2)operator_layout_update(9,"VERY_LONG_LAYER_NAME",85,true,batteries,valid,120,data.modifier_flags,false,3,false,true);
                    if(style==3)radii_layout_update(9,"VERY_LONG_LAYER_NAME",85,true,72,true,data.modifier_flags,false,3);
                    if(style==5)industrial_layout_update(&data,"VERY_LONG_LAYER_NAME");
                    if(style==6)segment_layout_update(&data,"VERY_LONG_LAYER_NAME");
                    if(style>=7&&style<=9)dashboard_layout_update(&data,"VERY_LONG_LAYER_NAME");
                    if(style>=10)concept_layout_update(&data,"VERY_LONG_LAYER_NAME");
                    /* Palette changes must preserve the full DYA layer name. */
                    for(int repeat=0;repeat<4;repeat++)prospector_layouts_cycle_palette();
                }
                for(int t=0;t<5;t++){host_ticks+=100;lv_tick_inc(100);lv_timer_handler();}
                lv_obj_update_layout(screen);text_count=0;inspect(screen,names[style-1]);check_overlaps(names[style-1]);
                if(style>=5)assert(verify_connection_icon(screen,data.usb_connected?LV_SYMBOL_USB:data.ble_connected?LV_SYMBOL_BLUETOOTH:LV_SYMBOL_CLOSE)==1);
                if(style>=5)assert(verify_modifiers(screen,data.modifier_flags,style)==4);
                if(style==7) {
                    lv_obj_t *dash=lv_obj_get_child(screen,0);int gauges=0;
                    for(uint32_t k=0;k<lv_obj_get_child_count(dash);k++) {
                        lv_obj_t *o=lv_obj_get_child(dash,k);
                        if(lv_obj_check_type(o,&lv_arc_class)&&lv_obj_get_y(o)==65) {
                            bool right=lv_obj_get_x(o)==160;
                            assert(lv_arc_get_mode(o)==(right?LV_ARC_MODE_REVERSE:LV_ARC_MODE_NORMAL));
                            unsigned level=right?data.peripheral_battery[0]:data.battery_level;
                            assert(lv_arc_get_value(o)==(int)(level<=100?level:0));gauges++;
                        }
                    }
                    assert(gauges==2);
                }
                if(style>=5 && state!=3) {
                    lv_refr_now(d); unsigned before=flush_count;
                    for(int repeat=0;repeat<10;repeat++) {
                        prospector_layouts_update(&data);lv_refr_now(d);
                    }
                    assert(flush_count==before); /* Idle snapshots must not redraw. */
                }
                if(palette==0&&argc>1) {
                    lv_refr_now(d);char path[1024];snprintf(path,sizeof(path),"%s/%s-%d.ppm",argv[1],names[style-1],state);
                    FILE*f=fopen(path,"wb");fprintf(f,"P6\n280 240\n255\n");fwrite(frame,1,sizeof(frame),f);fclose(f);
                }
                if(style==8&&palette==0&&state==1) {
                    const char *short_names[]={"SYS","NAV"};
                    for(int k=0;k<2;k++) {
                        dashboard_layout_update(&data,short_names[k]);lv_obj_update_layout(screen);
                        text_count=0;inspect(screen,"split");check_overlaps("split");lv_refr_now(d);
                        if(argc>1){char path[1024];snprintf(path,sizeof(path),"%s/split-%s.ppm",argv[1],short_names[k]);FILE*f=fopen(path,"wb");assert(f);fprintf(f,"P6\n280 240\n255\n");fwrite(frame,1,sizeof(frame),f);fclose(f);}
                    }
                }
            }
            prospector_layouts_cycle_palette();
        }
        lv_screen_load(parking_screen);
        prospector_layouts_destroy();lv_obj_delete(screen);
    }
    /* A real 60-second sampling trace; fixture values are not production data. */
    lv_obj_t *trace_screen=lv_obj_create(NULL);lv_obj_remove_style_all(trace_screen);
    lv_obj_set_size(trace_screen,280,240);lv_screen_load(trace_screen);prospector_layouts_init(trace_screen);
    prospector_layouts_set_style(PROSPECTOR_LAYOUT_MONO);
    struct prospector_keyboard_data trace={.usb_connected=true,.has_dynamic_data=true,.battery_level=83,.peripheral_battery={79,0,0},.modifier_flags=0x0a};
    memcpy(trace.current_layer_name,"BASE",5);
    const uint8_t wave[]={0,2,6,9,20,32,45,60,48,25,16,22,41,58,73,88,70,56,37,30,18,0,0,5,21,42,62,80,95,76,59,42};
    for(unsigned k=0;k<60;k++) {
        trace.wpm_value=wave[k%32];prospector_layouts_update(&trace);
        host_ticks+=1000;lv_tick_inc(1000);lv_timer_handler();
    }
    trace.wpm_value=42;prospector_layouts_update(&trace);lv_refr_now(d);
    lv_obj_update_layout(trace_screen);text_count=0;inspect(trace_screen,"mono-curve");check_overlaps("mono-curve");
    if(argc>1){char path[1024];snprintf(path,sizeof(path),"%s/mono-curve.ppm",argv[1]);FILE*f=fopen(path,"wb");fprintf(f,"P6\n280 240\n255\n");fwrite(frame,1,sizeof(frame),f);fclose(f);}
    unsigned orange=0;
    for(int y=145;y<170;y++)for(int x=22;x<186;x++) {
        unsigned offset=(y*280+x)*3;
        if(frame[offset]>200&&frame[offset+1]>90&&frame[offset+1]<190&&frame[offset+2]<100)orange++;
    }
    assert(orange>20); /* The sampled waveform must actually be drawn. */
    for(int k=0;k<60;k++){lv_tick_inc(1000);host_ticks+=1000;lv_timer_handler();}
    lv_refr_now(d);unsigned flat_before=flush_count;
    lv_tick_inc(1000);host_ticks+=1000;lv_timer_handler();lv_refr_now(d);
    assert(flush_count==flat_before); /* A full flat history does not repaint. */
    lv_screen_load(parking_screen);prospector_layouts_destroy();lv_obj_delete(trace_screen);
    lv_obj_t *terminal_screen=lv_obj_create(NULL);lv_obj_remove_style_all(terminal_screen);
    lv_obj_set_size(terminal_screen,280,240);lv_screen_load(terminal_screen);prospector_layouts_init(terminal_screen);
    prospector_layouts_set_style(PROSPECTOR_LAYOUT_TERMINAL);
    for(int k=0;k<60;k++){trace.wpm_value=wave[k%32];prospector_layouts_update(&trace);lv_tick_inc(1000);host_ticks+=1000;lv_timer_handler();}
    trace.wpm_value=42;prospector_layouts_update(&trace);lv_refr_now(d);unsigned green=0;
    for(int y=174;y<209;y++)for(int x=24;x<256;x++){unsigned at=(y*280+x)*3;if(frame[at+1]>150&&frame[at]<200&&frame[at+2]<180)green++;}
    assert(green>20);
    if(argc>1){char path[1024];snprintf(path,sizeof(path),"%s/terminal-history.ppm",argv[1]);FILE*f=fopen(path,"wb");fprintf(f,"P6\n280 240\n255\n");fwrite(frame,1,sizeof(frame),f);fclose(f);}
    for(int k=0;k<60;k++){lv_tick_inc(1000);host_ticks+=1000;lv_timer_handler();}
    lv_refr_now(d);unsigned terminal_flat=flush_count;lv_tick_inc(1000);host_ticks+=1000;lv_timer_handler();lv_refr_now(d);assert(flush_count==terminal_flat);
    dashboard_layout_reset_history();lv_refr_now(d);unsigned cleared=flush_count;lv_tick_inc(1000);host_ticks+=1000;lv_timer_handler();lv_refr_now(d);assert(flush_count==cleared);
    lv_screen_load(parking_screen);prospector_layouts_destroy();lv_obj_delete(terminal_screen);
    lv_obj_t *dya_screen=lv_obj_create(NULL);lv_obj_remove_style_all(dya_screen);
    lv_obj_set_size(dya_screen,280,240);prospector_layouts_init(dya_screen);test_dya_contract();
    prospector_layouts_destroy();lv_obj_delete(dya_screen);
    printf("288 layout states checked; %d foreground rectangles; unsafe: %d; clipped: %d; overlapping: %d\n",inspected,failures,clipping_failures,overlap_failures);
    return failures||clipping_failures||overlap_failures?1:0;
}
