USER_OBJECTS = $(BUILD_DIR)/sysroot/entry.o \
	$(BUILD_DIR)/kernel/ulibc.o \
	$(BUILD_DIR)/kernel/ulibs.o

$(BUILD_DIR)/sysroot/%.o: sysroot/%.S
	@mkdir -p $(@D)
	$(CPP) $(CPPFLAGS) -o $(@D)/$(*F).s sysroot/$(*F).S
	$(AS) $(ASFLAGS) -o $(@D)/$(*F).o $(@D)/$(*F).s -a=$(@D)/$(*F).lst
	$(RM) -f $(@D)/$(*F).s

$(BUILD_DIR)/sysroot/%.o: sysroot/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $(@D)/$(*F).o -c sysroot/$(*F).c

$(BUILD_DIR)/sysroot/%.elf: $(BUILD_DIR)/sysroot/%.o $(USER_OBJECTS)
	$(LD) -melf_i386 -pie -o $(@D)/$(*F).elf -Ttext 0x12345000 $(USER_OBJECTS) $(BUILD_DIR)/sysroot/$(*F).o

idle: $(BUILD_DIR)/sysroot/idle.elf
main1: $(BUILD_DIR)/sysroot/main1.elf
main2: $(BUILD_DIR)/sysroot/main2.elf
main3: $(BUILD_DIR)/sysroot/main3.elf
main4: $(BUILD_DIR)/sysroot/main4.elf
main5: $(BUILD_DIR)/sysroot/main5.elf
main6: $(BUILD_DIR)/sysroot/main6.elf
userH: $(BUILD_DIR)/sysroot/userH.elf
userI: $(BUILD_DIR)/sysroot/userI.elf
userJ: $(BUILD_DIR)/sysroot/userJ.elf
userP: $(BUILD_DIR)/sysroot/userP.elf
userQ: $(BUILD_DIR)/sysroot/userQ.elf
userR: $(BUILD_DIR)/sysroot/userR.elf
userS: $(BUILD_DIR)/sysroot/userS.elf
userV: $(BUILD_DIR)/sysroot/userV.elf
userW: $(BUILD_DIR)/sysroot/userW.elf
userX: $(BUILD_DIR)/sysroot/userX.elf
userY: $(BUILD_DIR)/sysroot/userY.elf
userZ: $(BUILD_DIR)/sysroot/userZ.elf

user: idle main1 main2 main3 main4 main5 main6 userH userI userJ userP userQ userR userS userV userW userX userY userZ