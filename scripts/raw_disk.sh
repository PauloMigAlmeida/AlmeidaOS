#!/bin/bash
rm -f /code/build/disk.img
dd if=/code/build/boot/mbr.bin of=/code/build/disk.img bs=512 count=1 conv=notrunc
dd if=/code/build/boot/loader.bin of=/code/build/disk.img bs=512 count=5 seek=1 conv=notrunc
dd if=/code/build/kernel/kernel.bin of=/code/build/disk.img bs=512 count=100 seek=6 conv=notrunc
