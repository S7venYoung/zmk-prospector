#!/usr/bin/env python3
"""Compile real LVGL + the original renderers; no west/SDK/firmware build.
Usage: run.py LVGL_SOURCE KBLADE_MODULE OUTPUT_DIR.
"""
import concurrent.futures, os, pathlib, subprocess, sys
root=pathlib.Path(__file__).resolve().parents[2]
lvgl, module, out=map(lambda s:pathlib.Path(s).resolve(),sys.argv[1:])
out.mkdir(parents=True,exist_ok=True)
shared=module/'boards/shields/src'
flags=['-std=c99','-O0','-g','-DLV_CONF_SKIP','-DLV_COLOR_DEPTH=24',
       '-DCONFIG_ZMK_PROSPECTOR_DYA_SETTINGS=0','-DCONFIG_ZMK_LOG_LEVEL=1','-DCONFIG_PROSPECTOR_DEFAULT_LAYOUT=2',
       '-I'+str(lvgl),'-I'+str(root/'tests/ms2_host/stubs'),
       '-I'+str(root/'themes/scanner-original/src'),'-I'+str(shared)]
sources=list((lvgl/'src').rglob('*.c'))
sources += [p for p in (root/'themes/scanner-original/src').glob('*.c') if p.name not in {'yads_input.c', 'dya_settings.c', 'dya_battery_bridge.c'}]
sources+=list((shared/'fonts_carrefinho').glob('*.c'))
sources += [root/'tests/ms2_host/harness.c', root/'tests/ms2_host/dya_contract.c']
headers = list((root/'themes/scanner-original/src').glob('*.h'))
headers += list((root/'tests/ms2_host/stubs').rglob('*.h'))
headers += list((lvgl/'src').rglob('*.h'))
headers += list(shared.glob('*.h'))
dependency_mtime = max([Path.stat().st_mtime for Path in headers] + [pathlib.Path(__file__).stat().st_mtime])
def compile_one(item):
    i,src=item; import hashlib; obj=out/(hashlib.sha1(str(src).encode()).hexdigest()+'.o')
    if not obj.exists() or obj.stat().st_mtime < max(src.stat().st_mtime, dependency_mtime):
        proc=subprocess.run(['clang',*flags,'-c',str(src),'-o',str(obj)],capture_output=True,text=True)
        if proc.returncode:raise RuntimeError(str(src)+'\n'+proc.stderr)
    return str(obj)
with concurrent.futures.ThreadPoolExecutor(max_workers=8) as pool:
    objects=list(pool.map(compile_one,enumerate(sources)))
subprocess.run(['clang',*objects,'-lm','-o',str(out/'check-layouts')],check=True)
subprocess.run([str(out/'check-layouts'),str(out)],check=True)
