/* SPDX-License-Identifier: MIT; advertisements contain no typed letters. */
#include "yads_input.h"
#include <stdio.h>
void yads_input_enable(bool enabled){(void)enabled;}
void yads_input_text(uint8_t layer,const char *name,char out[32]) {
    if(name&&*name)snprintf(out,32,"%s",name);else snprintf(out,32,"L%u",layer);
}
