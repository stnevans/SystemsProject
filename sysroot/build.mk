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

user: idle