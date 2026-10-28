#pragma once
#include <stdint.h>
#include <string.h>
typedef struct {uint8_t val[6];} bt_addr_t;
typedef struct {uint8_t type;bt_addr_t a;} bt_addr_le_t;
static inline int bt_addr_le_cmp(const bt_addr_le_t*a,const bt_addr_le_t*b){return memcmp(a,b,sizeof(*a));}
static inline void bt_addr_le_copy(bt_addr_le_t*a,const bt_addr_le_t*b){*a=*b;}
struct bt_le_scan_param {uint8_t type,options;uint16_t interval,window;};
struct net_buf_simple;
static inline int bt_le_scan_start(const struct bt_le_scan_param*p,void(*cb)(const bt_addr_le_t*,int8_t,uint8_t,struct net_buf_simple*)){(void)p;(void)cb;return 0;}
static inline int bt_le_scan_stop(void){return 0;}
#define BT_DATA_NAME_SHORTENED 8
#define BT_DATA_NAME_COMPLETE 9
#define BT_DATA_MANUFACTURER_DATA 255
#define BT_LE_SCAN_TYPE_ACTIVE 1
#define BT_LE_SCAN_OPT_NONE 0
#define BT_GAP_SCAN_FAST_WINDOW 96
#define BT_HCI_LE_ADV_EVT_TYPE_SCAN_RSP 4
