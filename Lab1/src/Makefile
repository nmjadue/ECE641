# For Linux or any machines with gcc compiler
CC = gcc
CFLAGS = -std=c99 -Wall -pedantic
BIN = ../bin

all: Number01 Number01_04 Number03 Number03_4 Number03_05 Number04_1 Number04_2

clean:
	/bin/rm *.o $(BIN)/*

OBJ = tiff.o allocate.o randlib.o qGGMRF.o solve.o

Number01: Number01.o $(OBJ)
	$(CC) $(CFLAGS) -o Number01 Number01.o $(OBJ) -lm
	mv Number01 $(BIN)

Number01_04: Number01_04.o $(OBJ)
		$(CC) $(CFLAGS) -o Number01_04 Number01_04.o $(OBJ) -lm
		mv Number01_04 $(BIN)

Number03: Number03.o $(OBJ)
		$(CC) $(CFLAGS) -o Number03 Number03.o $(OBJ) -lm
		mv Number03 $(BIN)

Number03_4: Number03_4.o $(OBJ)
		$(CC) $(CFLAGS) -o Number03_4 Number03_4.o $(OBJ) -lm
		mv Number03_4 $(BIN)

Number03_05: Number03_05.o $(OBJ)
		$(CC) $(CFLAGS) -o Number03_05 Number03_05.o $(OBJ) -lm
		mv Number03_05 $(BIN)

Number04_1: Number04_1.o $(OBJ)
		$(CC) $(CFLAGS) -o Number04_1 Number04_1.o $(OBJ) -lm
		mv Number04_1 $(BIN)

Number04_2: Number04_2.o $(OBJ)
		$(CC) $(CFLAGS) -o Number04_2 Number04_2.o $(OBJ) -lm
		mv Number04_2 $(BIN)
