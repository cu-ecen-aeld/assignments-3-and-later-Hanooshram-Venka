
#file - makefile
#Author - Hanooshram Venkateswaran



CROSS_COMPILE ?=     #this is for conditional compiling
CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g #For compiler to show all warnings, -g for debugging
TARGET = writer
SRC = writer.c
OBJ = $(SRC:.c=.o) #for turning writer.c into writer.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)  #for creating of the 'writer' binary from the .o file

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)  #takes the .c file and created .o file

clean:
	rm -f $(TARGET) $(OBJ)
