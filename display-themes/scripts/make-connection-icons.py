#!/usr/bin/env python3
"""Original geometric USB/Bluetooth/off glyphs, 2bpp; Pillow to regenerate only."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[1]
bitmap=bytearray();desc=[(0,0,0,0,0,0)];cps=[0xf00d,0xf287,0xf293]
for cp in cps:
 im=Image.new('L',(64,72));d=ImageDraw.Draw(im)
 def line(points):d.line([(x*4,y*4) for x,y in points],fill=255,width=6,joint='curve')
 if cp==0xf00d:line([(3,4),(12,14)]);line([(12,4),(3,14)])
 elif cp==0xf287:
  line([(8,15),(8,2)]);d.polygon([(8*4,0),(5*4,4*4),(11*4,4*4)],fill=255)
  line([(8,12),(3,9),(3,6)]);d.ellipse((1*4,4*4,5*4,8*4),fill=255)
  line([(8,9),(13,6),(13,4)]);d.rectangle((11*4,2*4,15*4,6*4),fill=255)
  d.ellipse((6*4,14*4,10*4,18*4),fill=255)
 else:
  line([(7,1),(7,17),(13,12),(2,4)]);line([(2,14),(13,6),(7,1)])
 vals=[round(v/85) for v in im.resize((16,18),Image.Resampling.LANCZOS).tobytes()];off=len(bitmap)
 bitmap.extend((vals[i]<<6)|(vals[i+1]<<4)|(vals[i+2]<<2)|vals[i+3] for i in range(0,len(vals),4));desc.append((off,18*16,16,18,0,1))
s=['/* SPDX-License-Identifier: MIT; original geometric connection icons. */','#include <lvgl.h>','static const uint8_t bitmap[]={']
for i in range(0,len(bitmap),16):s.append(','.join('0x%02x'%v for v in bitmap[i:i+16])+',')
s+=['};','static const lv_font_fmt_txt_glyph_dsc_t glyphs[]={']
s+=['{.bitmap_index=%d,.adv_w=%d,.box_w=%d,.box_h=%d,.ofs_x=%d,.ofs_y=%d},'%v for v in desc]
s+=['};','static const uint16_t unicode[]={0,634,646};','static const lv_font_fmt_txt_cmap_t cmaps[]={{.range_start=61453,.range_length=647,.glyph_id_start=1,.unicode_list=unicode,.list_length=3,.type=LV_FONT_FMT_TXT_CMAP_SPARSE_TINY}};','static const lv_font_fmt_txt_dsc_t dsc={.glyph_bitmap=bitmap,.glyph_dsc=glyphs,.cmaps=cmaps,.cmap_num=1,.bpp=2};','const lv_font_t connection_icons_20={.get_glyph_dsc=lv_font_get_glyph_dsc_fmt_txt,.get_glyph_bitmap=lv_font_get_bitmap_fmt_txt,.line_height=20,.base_line=1,.dsc=&dsc};']
# Sparse codepoints are relative to F00D, not a contiguous icon block.
s=[v.replace('0,634,646','0,'+str(0xf287-0xf00d)+','+str(0xf293-0xf00d)).replace('.range_length=647','.range_length='+str(0xf293-0xf00d+1)) for v in s]
(root/'src/connection_icons_20.c').write_text('\n'.join(s)+'\n')
