#include <assert.h>
#include <string.h>
#include "yads_history.h"
int main(void) {
    struct yads_history h;char text[32];yads_history_reset(&h);
    yads_history_text(&h,0,"BASE",0,text);assert(!strcmp(text,"BASE"));
    for(int i=0;i<6;i++)yads_history_key(&h,4+i,true,0,100+i);
    yads_history_text(&h,0,"BASE",106,text);assert(!strcmp(text,"BCDEF"));
    yads_history_key(&h,0x2a,true,0,200);
    yads_history_text(&h,0,"BASE",201,text);assert(!strcmp(text,"BCDE"));
    yads_history_key(&h,0xe3,true,0,300);
    yads_history_text(&h,0,"BASE",301,text);assert(!strcmp(text,""));
    yads_history_key(&h,6,true,0,302);
    yads_history_text(&h,0,"BASE",303,text);assert(!strcmp(text,"C"));
    yads_history_key(&h,6,false,0,304);
    yads_history_text(&h,0,"BASE",305,text);assert(!strcmp(text,""));
    yads_history_key(&h,0xe3,false,0,306);assert(h.modifiers==0);
    yads_history_key(&h,4,true,2,400);
    yads_history_text(&h,2,"NAV",401,text);assert(!strcmp(text,"NAV A"));
    yads_history_text(&h,2,"NAV",5400,text);assert(!strcmp(text,"NAV"));
    yads_history_text(&h,0,"BASE",5401,text);assert(!strcmp(text,"BASE"));
    yads_history_key(&h,5,true,0,0xfffffff0u);
    yads_history_text(&h,0,"BASE",0x1380u,text);assert(!strcmp(text,"BASE"));
    yads_history_reset(&h);assert(h.length==0&&h.modifiers==0);
    return 0;
}
