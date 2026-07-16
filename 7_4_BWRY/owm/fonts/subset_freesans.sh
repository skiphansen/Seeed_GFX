#!/bin/sh

set -x
# extract just the "extended ASCII aka ISO 8859 aka ISO Latin-1 plus 
# the code points from Window code page 1252 in the rannge of
# 0x80 -> 0xbf
fonttools subset FreeSans.otf --output-file=FreeSans_subset.otf --unicodes="U+20-ff,U+152-153,U+160-161,U+178,U+17D,U+17E,U+192,U+2C6,U+2DC,U+2013,U+2014,U+2018,U+2019,U+201A,U+201C-201E,U+2020-2022,U+2026,U+2030,U+2039,U+203A,U+20AC,U+2122"

# remap cp 1252 into 0x80 -> 0xbf
./AddExtendedAscii.py


