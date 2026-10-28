#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "prospector_layouts.h"
#include "yads_history.h"
#include "yads_input.h"
#include "field_layout.h"
#include "operator_layout.h"
#include "radii_layout.h"
#include "industrial_layout.h"
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
static uint8_t frame[280 * 240 * 3];
static uint8_t draw_buf[280 * 32 * 3] __attribute__((aligned(64)));
static void flush(lv_display_t *d, const lv_area_t *a, uint8_t *p) {
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
    if(lv_obj_check_type(o,&lv_arc_class) || lv_obj_check_type(o,&lv_image_class) || lv_obj_check_type(o,&lv_bar_class)) {
        lv_area_t a;lv_obj_get_coords(o,&a);inspected++;
        if(!safe(a.x1,a.y1)||!safe(a.x2,a.y1)||!safe(a.x1,a.y2)||!safe(a.x2,a.y2)) {
            printf("UNSAFE %s indicator bounds=%d,%d,%d,%d\n",theme,a.x1,a.y1,a.x2,a.y2);failures++;
        }
    }
    if(lv_obj_check_type(o,&lv_label_class)) {
        const char *text=lv_label_get_text(o);
        if(text && *text && lv_obj_get_style_text_opa(o,0)>0) {
            lv_area_t a;lv_obj_get_coords(o,&a);inspected++;
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
static int verify_industrial_modifiers(lv_obj_t *o, uint8_t flags) {
    const char *symbols[] = {SYMBOL_COMMAND, SYMBOL_OPTION, SYMBOL_CONTROL, SYMBOL_SHIFT};
    const uint8_t masks[] = {0x88, 0x44, 0x11, 0x22};
    int found = 0;
    if (lv_obj_check_type(o, &lv_label_class)) {
        for (int i = 0; i < 4; i++) if (!strcmp(lv_label_get_text(o), symbols[i])) {
            bool active = (flags & masks[i]) != 0;
            assert(lv_color_eq(lv_obj_get_style_bg_color(lv_obj_get_parent(o), 0),
                lv_color_hex(active ? 0xFFCA13 : 0x1B2420)));
            assert(lv_color_eq(lv_obj_get_style_text_color(o, 0),
                lv_color_hex(active ? 0x0D1310 : 0x777C76)));
            found++;
        }
    }
    for(uint32_t i=0;i<lv_obj_get_child_count(o);i++)
        found += verify_industrial_modifiers(lv_obj_get_child(o,i), flags);
    return found;
}
int main(int argc,char **argv) {
    setbuf(stdout,NULL);
    lv_init();lv_display_t *d=lv_display_create(280,240);
    lv_display_set_color_format(d,LV_COLOR_FORMAT_RGB888);
    lv_display_set_buffers(d,draw_buf,NULL,sizeof(draw_buf),LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(d,flush);
    lv_obj_t *parking_screen=lv_screen_active();
    /* Check forward/reverse switching while live renderers/timers exist. */
    lv_obj_t *cycle_screen=lv_obj_create(NULL);lv_obj_remove_style_all(cycle_screen);
    lv_obj_set_size(cycle_screen,280,240);prospector_layouts_init(cycle_screen);
    prospector_layouts_set_style(PROSPECTOR_LAYOUT_FIELD);
    for(int n=0;n<25;n++) {
        int expected=(n%5)+1;assert(prospector_layouts_get_style()==expected);
        prospector_layouts_next();
    }
    for(int n=0;n<25;n++) {
        prospector_layouts_prev();int expected=5-(n%5);
        assert(prospector_layouts_get_style()==expected);
    }
    prospector_layouts_destroy();lv_obj_delete(cycle_screen);
    const char *names[]={"field","operator","radii","yads","industrial"};
    for(int style=1;style<=5;style++) {
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
                memcpy(data.current_layer_name,state==3?"WWWWWWWW":"BASE",8);
                if(style==5 && (palette & 1)) data.modifier_flags <<= 4;
                if(style==5 && state==1) {
                    data.active_layer=0; data.wpm_value=42;
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
                    /* Palette changes must preserve the full DYA layer name. */
                    for(int repeat=0;repeat<4;repeat++)prospector_layouts_cycle_palette();
                }
                for(int t=0;t<5;t++){host_ticks+=100;lv_tick_inc(100);lv_timer_handler();}
                lv_obj_update_layout(screen);text_count=0;inspect(screen,names[style-1]);check_overlaps(names[style-1]);
                if(style==5)assert(verify_industrial_modifiers(screen,data.modifier_flags)==4);
                if(palette==0&&argc>1) {
                    lv_refr_now(d);char path[1024];snprintf(path,sizeof(path),"%s/%s-%d.ppm",argv[1],names[style-1],state);
                    FILE*f=fopen(path,"wb");fprintf(f,"P6\n280 240\n255\n");fwrite(frame,1,sizeof(frame),f);fclose(f);
                }
            }
            prospector_layouts_cycle_palette();
        }
        lv_screen_load(parking_screen);
        prospector_layouts_destroy();lv_obj_delete(screen);
    }
    lv_obj_t *dya_screen=lv_obj_create(NULL);lv_obj_remove_style_all(dya_screen);
    lv_obj_set_size(dya_screen,280,240);prospector_layouts_init(dya_screen);test_dya_contract();
    prospector_layouts_destroy();lv_obj_delete(dya_screen);
    printf("120 layout states checked; %d foreground rectangles; unsafe: %d; clipped: %d; overlapping: %d\n",inspected,failures,clipping_failures,overlap_failures);
    return failures||clipping_failures||overlap_failures?1:0;
}
