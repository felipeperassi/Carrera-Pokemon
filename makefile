VALGRIND_FLAGS=--leak-check=full --track-origins=yes --show-reachable=yes --error-exitcode=2 --show-leak-kinds=all --trace-children=yes
CFLAGS =-std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O2 -g
CC = gcc

all: clean valgrind-juego

valgrind_alumno: compilar_pruebas_alumno
	valgrind $(VALGRIND_FLAGS) ./pruebas_alumno

tp: src/*.c tp.c
	$(CC) $(CFLAGS) src/*.c tp.c -o tp

pruebas_alumno: compilar_pruebas_alumno
	./pruebas_alumno

compilar_pruebas_alumno: src/*.c pruebas_alumno.c
	$(CC) $(CFLAGS) src/*.c pruebas_alumno.c -o pruebas_alumno

valgrind_juego_default: compilar_juego
	valgrind $(VALGRIND_FLAGS) ./juego "archivos/ejemplo.txt"

juego_default: compilar_juego
	./juego "archivos/ejemplo.txt"

compilar_juego: src/*.c juego.c
	$(CC) $(CFLAGS) src/*.c juego.c -o juego

clean:
	rm -f pruebas_alumno pruebas_chanutron juego
