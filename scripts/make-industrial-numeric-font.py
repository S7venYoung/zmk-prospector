#!/usr/bin/env python3
"""Compatibility entry point for the extracted theme module."""
from pathlib import Path
import runpy
runpy.run_path(str(Path(__file__).resolve().parents[1]/"display-themes/scripts/make-industrial-fonts.py"), run_name="__main__")
