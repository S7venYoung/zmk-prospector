#!/usr/bin/env python3
"""Compatibility entry point for the complete INDUSTRIAL font generator."""
from pathlib import Path
import runpy
runpy.run_path(str(Path(__file__).with_name('make-industrial-fonts.py')), run_name='__main__')
