#!/bin/sh
PLATFORM=/Developer/Platforms/iPhoneOS.platform

$PLATFORM/Developer/usr/bin/arm-apple-darwin10-llvm-gcc-4.2 $@ -static -nostartfiles -nodefaultlibs -nostdlib -Wl,-e,_main
