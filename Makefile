# -*- MakeFile -*-
CC=gcc 	#set compilter to gcc
CFLAGS=-lm # a flag that link to the math library
TARGET=run # name of output binary

all:
	$(CC) main.c Sources/img_operations.c Sources/utils.c Sources/pixles.c  Sources/linked_list.c Sources/rst_moments.c Sources/morphology.c Sources/data_base.c Sources/edge_detection.c $(CFLAGS) -o $(TARGET) -g
clean:
	rm $(TARGET)
