CC=gcc 	#set compilter to gcc
CFLAGS=-lm
TARGET=run

all:
	$(CC) main.c Sources/img_operations.c Sources/utils.c Sources/pixles.c  Sources/linked_list.c Sources/rst_moments.c Sources/morphology.c Sources/data_base.c $(CFLAGS) -o $(TARGET) 
clean:
	rm $(TARGET)
