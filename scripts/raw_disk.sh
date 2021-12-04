#!/bin/bash -e

#################
# sanity checks #
#################

# check if kernel size is bigger then the number of blocks allocated 
# I will have to use this until I either implement a HD driver and implement a filesystem
kernel_size=$(stat -c%s /code/build/kernel/kernel)
if (( kernel_size > 77824 )); then
    echo "[ERROR] :: raw_disk.sh :: Kernel excceded the blocks allocated. Exiting..."
    exit 1
fi

# check if user progrm size is bigger then the number of blocks allocated 
# I will have to use this until I either implement a HD driver and implement a filesystem
user_size=$(stat -c%s /code/build/user/user)
if (( user_size > 7680 )); then
    echo "[ERROR] :: raw_disk.sh :: User program excceded the blocks allocated. Exiting..."
    exit 1
fi

rm -f /code/build/disk.img
dd if=/code/build/boot/mbr.bin of=/code/build/disk.img bs=512 count=1 conv=notrunc
dd if=/code/build/boot/loader.bin of=/code/build/disk.img bs=512 count=5 seek=1 conv=notrunc
dd if=/code/build/kernel/kernel of=/code/build/disk.img bs=512 count=152 seek=6 conv=notrunc
dd if=/code/build/user/user of=/code/build/disk.img bs=512 count=15 seek=158 conv=notrunc

# this addresses a bug in the qemu that fails to read data out of the disk.img
# if that terminates prematurely. In a real computer, this wouldn't be likely to
# happen as (assuming that the usb stick used has a bigger capacity then the file copied),
# BIOS would read garbage from whatever happens to be on the subsequent blocks.
truncate -s $(expr 512 \* 173) /code/build/disk.img
