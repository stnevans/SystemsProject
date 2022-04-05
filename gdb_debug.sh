gnome-terminal --tab -- bash -c 'gdb -ex="target rem:1234" -ex="file prog.o"'
/usr/bin/qemu-system-i386 -serial mon:stdio -drive file=usb.img,index=0,media=disk,format=raw -S -s 
