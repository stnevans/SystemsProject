GEN_OPTIONS = -DCLEAR_BSS -DGET_MMAP -DSP_OS_CONFIG
DBG_OPTIONS = -DTRACE_CX -DSTATUS=3

USER_OPTIONS = $(GEN_OPTIONS) $(DBG_OPTIONS)

INCLUDES = -Iinclude -I.

CPP = cpp
CPPFLAGS = $(USER_OPTIONS) -nostdinc $(INCLUDES)

CC = gcc
CFLAGS = -m32 -std=c99 -fno-stack-protector -fno-builtin -Wall -Wstrict-prototypes $(CPPFLAGS)

AS = as
ASFLAGS = --32

LD = ld
LDFLAGS = -melf_i386 -no-pie

BUILD_DIR = $(shell pwd)/build

-include util/build.mk
-include boot/build.mk
-include kernel/build.mk
-include sysroot/build.mk

$(BUILD_DIR)/usb.img: offsets.h bootstrap.b prog.b prog.nl BuildImage prog.dis user
	./BuildImage -d usb -o $(BUILD_DIR)/usb.img -b $(BUILD_DIR)/bootstrap.b $(BUILD_DIR)/prog.b 0x10000 $(BUILD_DIR)/sysroot/idle.elf 0x20000

$(BUILD_DIR)/floppy.img: bootstrap.b prog.b prog.nl BuildImage prog.dis 
	./BuildImage -d floppy -o $(BUILD_DIR)/floppy.img -b $(BUILD_DIR)/bootstrap.b $(BUILD_DIR)/prog.b 0x10000

floppy:	$(BUILD_DIR)/floppy.img

usb:	$(BUILD_DIR)/usb.img

clean:
	rm -rf $(BUILD_DIR)

realclean: clean
	rm -f BuildImage
	rm -f Offsets