#!/bin/bash -e

#################
# sanity checks #
#################

# check if kernel size is bigger then the number of blocks allocated 
# I will have to use this until I either implement a HD driver and implement a filesystem
kernel_size=$(stat -c%s /code/build/kernel/kernel)
if (( kernel_size > 76800 )); then
    echo "[ERROR] :: raw_disk.sh :: Kernel excceded the blocks allocated. Exiting..."
    exit 1
fi

rm -f /code/build/disk.img
dd if=/code/build/boot/mbr.bin of=/code/build/disk.img bs=512 count=1 conv=notrunc
dd if=/code/build/boot/loader.bin of=/code/build/disk.img bs=512 count=5 seek=1 conv=notrunc
dd if=/code/build/kernel/kernel of=/code/build/disk.img bs=512 count=150 seek=6 conv=notrunc

# this addresses a bug in the qemu that fails to read data out of the disk.img
# if that terminates prematurely. In a real computer, this wouldn't be likely to
# happen as (assuming that the usb stick used has a bigger capacity then the file copied),
# BIOS would read garbage from whatever happens to be on the subsequent blocks.
truncate -s $(expr 512 \* 156) /code/build/disk.img
