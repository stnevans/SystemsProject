KERNEL_SRCS = \
	$(wildcard kernel/*.c) 	

KERNEL_ASM_SRCS = \
	$(wildcard kernel/*.S)

KERNEL_OBJS = \
	$(patsubst %.c, $(BUILD_DIR)/%.o, $(KERNEL_SRCS)) \
	$(patsubst %.S, $(BUILD_DIR)/%.o, $(KERNEL_ASM_SRCS))

#
# Application files
#

OS_C_SRC = kernel/clock.c kernel/kernel.c kernel/kmem.c kernel/libc.c kernel/process.c kernel/queues.c kernel/scheduler.c \
	   kernel/sio.c kernel/stacks.c kernel/syscalls.c kernel/paging.c kernel/phys_alloc.c kernel/elf_loader.c \
	   kernel/ata.c kernel/filesystem.c
OS_C_OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(OS_C_SRC))

OS_S_SRC = kernel/libs.S
OS_S_OBJ = $(patsubst %.S, $(BUILD_DIR)/%.o, $(OS_S_SRC))

OS_SRCS  = $(OS_C_SRC) $(OS_S_SRC)
OS_OBJS  = $(OS_C_OBJ) $(OS_S_OBJ)

USR_C_SRC = kernel/users.c kernel/ulibc.c
USR_C_OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(USR_C_SRC)) 

USR_S_SRC = kernel/ulibs.S
USR_S_OBJ = $(patsubst %.S, $(BUILD_DIR)/%.o, $(USR_S_SRC))

USR_SRCS  = $(USR_C_SRC) $(USR_S_SRC)
USR_OBJS  = $(USR_C_OBJ) $(USR_S_OBJ)

FMK_S_SRC = kernel/startup.S kernel/isr_stubs.S
FMK_S_OBJ = $(patsubst %.S, $(BUILD_DIR)/%.o, $(FMK_S_SRC))

FMK_C_SRC = kernel/cio.c kernel/support.c
FMK_C_OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(FMK_C_SRC))

FMK_SRCS = $(FMK_S_SRC) $(FMK_C_SRC)
FMK_OBJS = $(FMK_S_OBJ) $(FMK_C_OBJ)

# Collections of files

OBJECTS = $(FMK_OBJS) $(OS_OBJS) $(USR_OBJS)

$(BUILD_DIR)/kernel/%.o: kernel/%.S
	@mkdir -p $(@D)
	$(CPP) $(CPPFLAGS) -o $(@D)/$(*F).s kernel/$(*F).S
	$(AS) $(ASFLAGS) -o $(@D)/$(*F).o $(@D)/$(*F).s -a=$(@D)/$(*F).lst
	$(RM) -f $(@D)/$(*F).s

$(BUILD_DIR)/kernel/%.o: kernel/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $(@D)/$(*F).o -c kernel/$(*F).c

$(BUILD_DIR)/prog.out: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)prog.out $(OBJECTS)

$(BUILD_DIR)/prog.o: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/prog.o -Ttext 0x10000 $(OBJECTS)

$(BUILD_DIR)/prog.b: $(BUILD_DIR)/prog.o
	objcopy --remove-section=.note.gnu.property -O binary $(BUILD_DIR)/prog.o $(BUILD_DIR)/prog.b

prog.b: $(BUILD_DIR)/prog.b

$(BUILD_DIR)/prog.nl: $(BUILD_DIR)/prog.o
	nm -Bng $(BUILD_DIR)/prog.o | pr -w80 -3 > $(BUILD_DIR)/prog.nl

prog.nl: $(BUILD_DIR)/prog.nl

$(BUILD_DIR)/prog.nll: $(BUILD_DIR)/prog.o
	nm -Bn $(BUILD_DIR)/prog.o | pr -w80 -3 > $(BUILD_DIR)/prog.nll

prog.nll: $(BUILD_DIR)/prog.nll

$(BUILD_DIR)/prog.dis: $(BUILD_DIR)/prog.o
	objdump -d $(BUILD_DIR)/prog.o > $(BUILD_DIR)/prog.dis

prog.dis: $(BUILD_DIR)/prog.dis