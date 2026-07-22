#!/bin/sh

set -x
fonttools subset MaterialIcons-Regular.ttf --output-file=subset.ttf --unicodes="U+EBDC,U+ebd9,U+ebe0,U+ebdd,U+ebe2,U+ebd4,U+ebd2,U+e1a4,U+e19c,U+e8f4"
ttx -f subset.ttf
#fonttools ttLib.scaleUpem subset.ttf 2048
#fonttools merge weathericons.ttf subset-scaled.ttf  --output-file=weathericons_plus.ttf




