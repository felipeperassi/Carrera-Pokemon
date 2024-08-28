#ifndef __MENU_H__
#define __MENU_H__

#include "tp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR -1

#define COLOR_RESET "\033[0m"
#define BLANCO "\x1b[37;1m"
#define VERDE "\x1b[32;1m"
#define ROJO "\x1b[31;1m"
#define AZUL "\033[1;34m"
#define AMARILLO "\x1b[33;1m"
#define NORMAL "\x1b[0m"

#define NEGRITA "\033[1m"
#define FORMATO_NORMAL "\033[49m"

enum MENU_DIFICULTAD { EASY, MEDIUM, HARD, IMPOSSIBLE };

struct menu {
	TP *tp;
	enum MENU_DIFICULTAD dificultad;
};

void borrar_pantalla();

void mover_a_posicion(int x, int y);

void pokemones_printear_disponibles(struct menu *menu, int x, int y);

void pokemon_printear_seleccionado(struct menu *menu, enum TP_JUGADOR jugador,
				   int x, int *y);

void menu_pantalla_inicial();

void menu_pantalla_principal();

void menu_pantalla_cambiar_archivo();

void menu_pantalla_dificultad();

void menu_pantalla_seleccion_pokemon_maquina(struct menu *menu);

void menu_pantalla_pista_pokemon_maquina(struct menu *menu);

void menu_pantalla_seleccion_pokemon_usuario(struct menu *menu);

void menu_pantalla_juego(struct menu *menu);

void menu_pantalla_pista_jugador(struct menu *menu);

void menu_pantalla_juego_informacion_oponente(struct menu *menu);

void menu_pantalla_juego_carrera(struct menu *menu);

void menu_pantalla_juego_agregar_obstaculo();

#endif /* __MENU_H__ */