/* Copyright (c) 2026 S7venYoung; SPDX-License-Identifier: MIT */
#include <zephyr/devicetree.h>
#include <zephyr/sys/util.h>

#if defined(CONFIG_USB_DEVICE_STACK)
/* The legacy CDC driver submits an OUT read and an IN ZLP for each enabled
 * interface during USB_DC_CONFIGURED. Allocation failures are not retried.
 * Count all CDCs, including the board's default and the logging snippet's.
 */
BUILD_ASSERT(CONFIG_USB_MAX_NUM_TRANSFERS >=
                 2 * DT_NUM_INST_STATUS_OKAY(zephyr_cdc_acm_uart),
             "Scanner USB transfer pool cannot cover all enabled CDC interfaces");
#endif
