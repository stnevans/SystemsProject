$(BUILD_DIR)/%.b: boot/%.S
	@mkdir -p $(@D)
	$(CPP) $(CPPFLAGS) -o $(@D)/$(*F).s boot/$(*F).S
	$(AS) $(ASFLAGS) -o $(@D)/$(*F).o $(@D)/$(*F).s -a=$(@D)/$(*F).lst
	$(LD) $(LDFLAGS) -Ttext 0x0 -s -e begtext -o $(@D)/$(*F).b $(@D)/$(*F).o
	objcopy --remove-section=.note.gnu.property -O binary $(@D)/$(*F).b

bootstrap.b: $(BUILD_DIR)/bootstrap.b