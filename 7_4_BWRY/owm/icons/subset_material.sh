#!/bin/sh

set -x
fonttools subset MaterialIcons-Regular.ttf --output-file=subset.ttf --unicodes-file=owm_material_codepoints.txt
ttx -f subset.ttf
#fonttools ttLib.scaleUpem subset.ttf 2048
#fonttools merge weathericons.ttf subset-scaled.ttf  --output-file=weathericons_plus.ttf




