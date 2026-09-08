#!/bin/sh

pio run --target upload --upload-port /dev/ttyACM0 2>&1 | tee e
if [ $? -eq 0 ]; then 
   ./run.sh
fi

