#!/bin/sh
PLATFORM=/Developer/Platforms/iPhoneOS.platform

$PLATFORM/Developer/usr/bin/arm-apple-darwin10-llvm-gcc-4.2 $@ -isysroot $PLATFORM/Developer/SDKs/iPhoneOS5.0.sdk -I$PLATFORM/Developer/SDKs/iPhoneOS5.0.sdk/usr/include -I.
