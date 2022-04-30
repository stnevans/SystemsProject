GEN_OPTIONS = -DCLEAR_BSS -DGET_MMAP -DSP_OS_CONFIG
DBG_OPTIONS = -DTRACE_CX -DSTATUS=3

USER_OPTIONS = $(GEN_OPTIONS) $(DBG_OPTIONS)

INCLUDES = -Iinclude -I.

CPP = cpp
CPPFLAGS = $(USER_OPTIONS) -nostdinc $(INCLUDES)

CC = gcc
CFLAGS = -m32 -std=c99 -fno-stack-protector -fno-builtin -Wall -Wstrict-prototypes $(CPPFLAGS) -g

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
	./BuildImage -d usb -o $(BUILD_DIR)/usb.img -b $(BUILD_DIR)/bootstrap.b $(BUILD_DIR)/prog.b 0x10000 \
	$(BUILD_DIR)/sysroot/idle.elf 0x23000 \
	$(BUILD_DIR)/sysroot/main1.elf 0x27000 \
	$(BUILD_DIR)/sysroot/main2.elf 0x2b000 \
	$(BUILD_DIR)/sysroot/main3.elf 0x2f000 \
	$(BUILD_DIR)/sysroot/main4.elf 0x33000 \
	$(BUILD_DIR)/sysroot/main5.elf 0x37000 \
	$(BUILD_DIR)/sysroot/main6.elf 0x3b000
# $(BUILD_DIR)/sysroot/userH.elf 0x3f000 
# $(BUILD_DIR)/sysroot/userI.elf 0x43000 \
# $(BUILD_DIR)/sysroot/userJ.elf 0x47000 \
# $(BUILD_DIR)/sysroot/userP.elf 0x4b000 \
# $(BUILD_DIR)/sysroot/userQ.elf 0x4f000 \
# $(BUILD_DIR)/sysroot/userR.elf 0x53000 \
# $(BUILD_DIR)/sysroot/userS.elf 0x57000 \
# $(BUILD_DIR)/sysroot/userV.elf 0x5b000 \
# $(BUILD_DIR)/sysroot/userW.elf 0x5f000 \
# $(BUILD_DIR)/sysroot/userX.elf 0x63000 \
# $(BUILD_DIR)/sysroot/userY.elf 0x67000 \
# $(BUILD_DIR)/sysroot/userZ.elf 0x6b000

$(BUILD_DIR)/floppy.img: bootstrap.b prog.b prog.nl BuildImage prog.dis 
	./BuildImage -d floppy -o $(BUILD_DIR)/floppy.img -b $(BUILD_DIR)/bootstrap.b $(BUILD_DIR)/prog.b 0x10000

floppy:	$(BUILD_DIR)/floppy.img

usb:	$(BUILD_DIR)/usb.img

clean:
	rm -rf $(BUILD_DIR)

realclean: clean
	rm -f BuildImage
	rm -f Offsets