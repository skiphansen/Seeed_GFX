#!/bin/sh

pio device monitor --port /dev/ttyACM0 2>&1 | tee e.log

