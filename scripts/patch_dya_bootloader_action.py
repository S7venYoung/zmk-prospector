#!/usr/bin/env python3
"""Install a guarded Studio action hook in the pinned ZMK keymap setter."""
from pathlib import Path
import sys

INCLUDE = '#include <zmk/keymap.h>'
ANCHOR = '    if (memcmp(&zmk_keymap[layer_id][storage_binding_idx], &binding, sizeof(binding)) == 0) {'
DECLARATION = '''
#if IS_ENABLED(CONFIG_PROSPECTOR_DYA_BOOTLOADER_ACTION)
int prospector_dya_binding_action(const struct zmk_behavior_binding *binding);
#endif'''
HOOK = '''    /* Prospector DYA bootloader action hook */
#if IS_ENABLED(CONFIG_PROSPECTOR_DYA_BOOTLOADER_ACTION)
    int action = prospector_dya_binding_action(&binding);
    if (action != 0) {
        return action < 0 ? action : 0;
    }
#endif

'''
MARKER = '/* Prospector DYA bootloader action hook */'

def patch(path):
    path = Path(path)
    source = path.read_text()
    if MARKER in source:
        if source.count(HOOK) != 1 or source.count(INCLUDE + DECLARATION) != 1:
            raise SystemExit(f'Incomplete or duplicate DYA bootloader hook: {path}')
        return
    if source.count(INCLUDE) != 1 or source.count(ANCHOR) != 1:
        raise SystemExit(f'Pinned ZMK keymap setter changed; inspect before patching: {path}')
    source = source.replace(INCLUDE, INCLUDE + DECLARATION, 1)
    source = source.replace(ANCHOR, HOOK + ANCHOR, 1)
    path.write_text(source)

if __name__ == '__main__':
    patch(sys.argv[1])
