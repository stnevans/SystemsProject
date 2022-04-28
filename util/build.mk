

BuildImage:	util/BuildImage.c
	$(CC) -o BuildImage util/BuildImage.c

Offsets:	util/Offsets.c include/process.h include/stacks.h include/queues.h include/common.h
	$(CC) -m32 -std=c99 $(INCLUDES) -o Offsets util/Offsets.c

offsets.h:	Offsets
	./Offsets -h
	mv offsets.h include/