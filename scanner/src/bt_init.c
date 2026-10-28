/* SPDX-License-Identifier: MIT */
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/init.h>
#include <errno.h>
static int scanner_bt_init(void) { int err=bt_enable(NULL); return err==-EALREADY?0:err; }
SYS_INIT(scanner_bt_init,APPLICATION,90);
