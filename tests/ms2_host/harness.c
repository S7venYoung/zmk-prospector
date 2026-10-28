#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "prospector_layouts.h"
#include "yads_history.h"
#include "yads_input.h"

uint32_t host_ticks;
uint8_t display_settings_get_max_layers(void) { return 10; }
/* The retained stock YADS2 renderer is not part of these three tests. */
lv_obj_t *yads2_layout_create(lv_obj_t *o) { return o; }
void yads2_layout_destroy(void) {}
void yads2_layout_update(uint8_t a,const char*b,uint8_t c,bool d,const uint8_t*e,
    const bool*f,uint8_t g,uint8_t h,bool i,uint8_t j,bool k,bool l,const char*m) {}

static struct yads_history input;
void yads_input_enable(bool enabled) { (void)enabled; yads_history_reset(&input); }
void yads_input_text(uint8_t layer,const char *name,char out[32]) {
    yads_history_text(&input,layer,name,host_ticks,out);
}
extern void test_dya_contract(void);
static uint8_t frame[280 * 240 * 3];
static uint8_t draw_buf[280 * 32 * 3];
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
static int inspected;
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
            if(!safe(a.x1,a.y1)||!safe(a.x2,a.y1)||!safe(a.x1,a.y2)||!safe(a.x2,a.y2)) {
                printf("UNSAFE %s text=%s bounds=%d,%d,%d,%d\n",theme,text,a.x1,a.y1,a.x2,a.y2);failures++;
            }
        }
    }
    for(uint32_t i=0;i<lv_obj_get_child_count(o);i++)inspect(lv_obj_get_child(o,i),theme);
}
int main(int argc,char **argv) {
    lv_init();lv_display_t *d=lv_display_create(280,240);
    lv_display_set_color_format(d,LV_COLOR_FORMAT_RGB888);
    lv_display_set_buffers(d,draw_buf,NULL,sizeof(draw_buf),LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(d,flush);
    /* Check forward/reverse switching while live renderers/timers exist. */
    lv_obj_t *cycle_screen=lv_obj_create(NULL);lv_obj_remove_style_all(cycle_screen);
    lv_obj_set_size(cycle_screen,280,240);prospector_layouts_init(cycle_screen);
    prospector_layouts_set_style(PROSPECTOR_LAYOUT_FIELD);
    for(int n=0;n<20;n++) {
        int expected=(n%4)+1;assert(prospector_layouts_get_style()==expected);
        prospector_layouts_next();
    }
    for(int n=0;n<20;n++) {
        prospector_layouts_prev();int expected=4-(n%4);
        assert(prospector_layouts_get_style()==expected);
    }
    prospector_layouts_destroy();lv_obj_delete(cycle_screen);
    const char *names[]={"field","operator","radii","yads"};
    for(int style=1;style<=4;style++) {
        lv_obj_t *screen=lv_obj_create(NULL);lv_obj_remove_style_all(screen);
        lv_obj_set_size(screen,280,240);lv_obj_set_style_bg_opa(screen,255,0);
        lv_obj_set_style_bg_color(screen,lv_color_black(),0);lv_screen_load(screen);
        prospector_layouts_init(screen);prospector_layouts_set_style(style);
        for(int palette=0;palette<4;palette++) {
            for(int state=0;state<4;state++) {
                struct prospector_keyboard_data data={.active_layer=state==3?9:state,
                    .wpm_value=state*40,.battery_level=state?85:0,.peripheral_battery={state==2?0:72,0,0},
                    .modifier_flags=state?0xff:0,.profile_slot=state,.usb_connected=state==1,
                    .ble_connected=state==2,.ble_bonded=state>1,.has_dynamic_data=true};
                memcpy(data.current_layer_name,state==3?"LONGNAME":"BASE",8);
                prospector_layouts_update(&data);
                for(int t=0;t<5;t++){host_ticks+=100;lv_tick_inc(100);lv_timer_handler();}
                lv_obj_update_layout(screen);inspect(screen,names[style-1]);
                if(palette==0&&state==1&&argc>1) {
                    lv_refr_now(d);char path[1024];snprintf(path,sizeof(path),"%s/%s.ppm",argv[1],names[style-1]);
                    FILE*f=fopen(path,"wb");fprintf(f,"P6\n280 240\n255\n");fwrite(frame,1,sizeof(frame),f);fclose(f);
                }
            }
            prospector_layouts_cycle_palette();
        }
        prospector_layouts_destroy();lv_obj_delete(screen);
    }
    lv_obj_t *dya_screen=lv_obj_create(NULL);lv_obj_remove_style_all(dya_screen);
    lv_obj_set_size(dya_screen,280,240);prospector_layouts_init(dya_screen);test_dya_contract();
    prospector_layouts_destroy();lv_obj_delete(dya_screen);
    printf("64 layout states checked; %d foreground rectangles; unsafe: %d\n",inspected,failures);
    return failures?1:0;
}
