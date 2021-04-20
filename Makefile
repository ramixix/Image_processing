CC=gcc 	#set compilter to gcc
CFLAGS=-lm
TARGET=run

all:
	$(CC) main.c Sources/img_operations.c Sources/utils.c Sources/pixles.c $(CFLAGS) -o $(TARGET) 
clean:
	rm $(TARGET)
