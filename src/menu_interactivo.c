#include "menu_interactivo.h"
#include "menu.h"
#include <ctype.h>

// ---------------------------------- Funciones Auxiliares para el manejo de pantalla -----------------------------------

void esperar_usuario()
{
	while (getchar() != '\n')
		;
}

static char *string_to_upper(const char *string)
{
	if (!string)
		return NULL;

	size_t tamanio = strlen(string);

	char *upper_string = calloc(tamanio + 1, sizeof(char));
	if (!upper_string)
		return NULL;

	for (size_t i = 0; i < tamanio; i++)
		upper_string[i] = (char)toupper(string[i]);

	upper_string[tamanio] = '\0';

	return upper_string;
}

static void pantalla_verificar_y_limpiar(int *y, int limite_y, int x,
					 void (*reconstruccion)())
{
	if (*y > limite_y) {
		borrar_pantalla();
		*y = 20;
		reconstruccion();
		mover_a_posicion(x, *y);
	}
}

static void pokemones_verificar_y_limpiar_pantalla(menu_t *menu, int *y,
						   int limite_y, int x)
{
	if (!menu)
		return;

	if (*y > limite_y) {
		borrar_pantalla();
		*y = 12;
		pokemones_printear_disponibles(menu, 6, 2);
		mover_a_posicion(x, *y);
	}
}

static void juego_verificar_y_limpiar_pantalla(menu_t *menu, int *y,
					       int limite_y, int x,
					       void (*reconstruccion)(menu_t *))
{
	if (*y > limite_y) {
		borrar_pantalla();
		*y = 21;
		reconstruccion(menu);
		mover_a_posicion(x, *y);
	}
}

// -------------------------------------- Funciones para interactuar con el usuario --------------------------------------

static int usuario_elige_main_menu(int x, int y)
{
	char opcion[20];
	int limite_y = 21;

	while (true) {
		mover_a_posicion(x, ++y);
		printf(NEGRITA "-> CHOOSE " AZUL "[1/2]: " COLOR_RESET);

		if (fgets(opcion, sizeof(opcion), stdin) != NULL) {
			opcion[strcspn(opcion, "\n")] = 0;

			if (strcmp(opcion, "1") == 0 ||
			    strcmp(opcion, "2") == 0) {
				borrar_pantalla();
				return atoi(opcion);

			} else {
				mover_a_posicion(x, ++y);
				pantalla_verificar_y_limpiar(
					&y, limite_y, x,
					menu_pantalla_principal);
				printf(ROJO NEGRITA
				       "   INVALID OPTION. PLEASE ENTER '1' OR '2'." COLOR_RESET);
			}
		}
	}

	return ERROR;
}

static bool usuario_cambia_pokemon(int x, int *y, int limite_y, menu_t *menu)
{
	char opcion[10];
	bool respuesta_valida = false;

	while (!respuesta_valida) {
		mover_a_posicion(x, ++(*y));
		printf(NEGRITA "-> ¿ARE YOU SURE? " AZUL
			       "[YES/NO]: " COLOR_RESET);

		if (fgets(opcion, sizeof(opcion), stdin) != NULL) {
			opcion[strcspn(opcion, "\n")] = 0;
			char *opcion_upper = string_to_upper(opcion);
			if (!opcion_upper)
				return false;

			if (strcmp(opcion_upper, "YES") == 0) {
				free(opcion_upper);
				return false;

			} else if (strcmp(opcion_upper, "NO") == 0) {
				respuesta_valida = true;
				pokemones_verificar_y_limpiar_pantalla(
					menu, y, limite_y, x);

			} else {
				mover_a_posicion(x, ++(*y));
				pokemones_verificar_y_limpiar_pantalla(
					menu, y, limite_y, x);
				printf(ROJO NEGRITA
				       "   INVALID OPTION. PLEASE ENTER 'YES' OR 'NO'." COLOR_RESET);
			}

			free(opcion_upper);
		}
	}

	return true;
}

static void pokemon_leer_nombre(char *nombre, int tamaño, int x, int y)
{
	mover_a_posicion(x, y);
	printf(NEGRITA "-> PLAYER 1: " COLOR_RESET);
	if (!fgets(nombre, tamaño, stdin)) {
		mover_a_posicion(x, ++y);
		printf(ROJO NEGRITA "   READING ERROR" COLOR_RESET);
		y++;
		return;
	}
	nombre[strcspn(nombre, "\n")] = 0;
}

static bool pokemon_se_selecciona(menu_t *menu, int x, int *y, char *nombre,
				  int limite_y)
{
	char *nombre_upper = string_to_upper(nombre);
	if (!nombre_upper)
		return false;

	char cadena_imprimir[50];

	if (!tp_seleccionar_pokemon(menu->tp, JUGADOR_1, nombre)) {
		mover_a_posicion(x, ++(*y));
		pokemones_verificar_y_limpiar_pantalla(menu, y, limite_y, x);
		strcpy(cadena_imprimir, "POKEMON NOT FOUND, TRY AGAIN.");

		if (strcmp(nombre_upper,
			   tp_pokemon_seleccionado(menu->tp, JUGADOR_2)
				   ->nombre) == 0)
			strcpy(cadena_imprimir,
			       "POKEMON IS TAKEN BY PLAYER 2, TRY ANOTHER ONE.");

		printf(ROJO NEGRITA "   %s" COLOR_RESET, cadena_imprimir);

		free(nombre_upper);
		return false;
	}

	free(nombre_upper);
	return true;
}

static void usuario_selecciona_pokemon(menu_t *menu, int x, int y)
{
	bool cambiar = true;
	int limite_y = 20;

	while (cambiar) {
		char nombre[50];
		bool se_selecciono = false;

		while (!se_selecciono) {
			pokemon_leer_nombre(nombre, sizeof(nombre), x, ++y);
			se_selecciono = pokemon_se_selecciona(menu, x, &y,
							      nombre, limite_y);
		}

		pokemon_printear_seleccionado(menu, JUGADOR_1, x, &y);

		cambiar = usuario_cambia_pokemon(x, &y, limite_y, menu);
	}
}

static enum MENU_DIFICULTAD usuario_elige_dificultad(int x, int y)
{
	char dificultad[20];
	int limite_y = 21;

	while (true) {
		mover_a_posicion(x, ++y);
		printf(NEGRITA "-> CHOOSE: " COLOR_RESET);

		if (fgets(dificultad, sizeof(dificultad), stdin) != NULL) {
			dificultad[strcspn(dificultad, "\n")] = 0;

			char *dificultad_upper = string_to_upper(dificultad);
			if (!dificultad_upper)
				return ERROR;

			enum MENU_DIFICULTAD seleccion;
			bool opcion_valida = true;

			if (strcmp(dificultad_upper, "EASY") == 0)
				seleccion = EASY;

			else if (strcmp(dificultad_upper, "MEDIUM") == 0)
				seleccion = MEDIUM;

			else if (strcmp(dificultad_upper, "HARD") == 0)
				seleccion = HARD;

			else if (strcmp(dificultad_upper, "IMPOSSIBLE") == 0)
				seleccion = IMPOSSIBLE;

			else {
				opcion_valida = false;
				mover_a_posicion(x, ++y);
				pantalla_verificar_y_limpiar(
					&y, limite_y, x,
					menu_pantalla_dificultad);
				printf(ROJO NEGRITA
				       "   INVALID OPTION. PLEASE ENTER ONE OF THE OPTIONS." COLOR_RESET);
			}

			free(dificultad_upper);

			if (opcion_valida)
				return seleccion;
		}
	}
	return ERROR;
}

static int usuario_elige_posicion(int x, int y)
{
	char opcion[20];
	bool respuesta_valida = false;
	int limite_y = 21;
	long numero;
	char *endptr;

	while (!respuesta_valida) {
		mover_a_posicion(x, ++y);
		printf(NEGRITA "-> CHOOSE POSITION " AZUL
			       "[NUMBER]: " COLOR_RESET);

		if (fgets(opcion, sizeof(opcion), stdin) != NULL) {
			opcion[strcspn(opcion, "\n")] = 0;
			numero = strtol(opcion, &endptr, 10);

			if (*endptr == '\0' && numero > 0) {
				return ((int)numero - 1);

			} else {
				mover_a_posicion(x, ++y);
				pantalla_verificar_y_limpiar(
					&y, limite_y, x,
					menu_pantalla_juego_agregar_obstaculo);
				printf(ROJO NEGRITA
				       "   INVALID OPTION. PLEASE ENTER A VALID NUMBER." COLOR_RESET);
			}
		}
	}

	return ERROR;
}

static void obstaculos_opcion_agregar(menu_t *menu, char *opcion_upper, int x,
				      int *y, int limite_y)
{
	int posicion = usuario_elige_posicion(x, *y);
	if (posicion == ERROR) {
		free(opcion_upper);
		return;
	}

	enum TP_OBSTACULO tipo_obstaculo;

	if (strcmp(opcion_upper, "F") == 0)
		tipo_obstaculo = OBSTACULO_FUERZA;

	else if (strcmp(opcion_upper, "D") == 0)
		tipo_obstaculo = OBSTACULO_DESTREZA;

	else
		tipo_obstaculo = OBSTACULO_INTELIGENCIA;

	tp_agregar_obstaculo(menu->tp, JUGADOR_1, tipo_obstaculo,
			     (unsigned int)posicion);
	mover_a_posicion(x, ++(*y));
	pantalla_verificar_y_limpiar(y, limite_y, x,
				     menu_pantalla_juego_agregar_obstaculo);
	printf(VERDE NEGRITA "   OBSTACLE CORRECTLY ADDED." COLOR_RESET);

	free(opcion_upper);
}

static void obstaculos_opcion_quitar(menu_t *menu, char *opcion_upper, int x,
				     int *y, int limite_y)
{
	int posicion = usuario_elige_posicion(x, *y);
	if (posicion == ERROR) {
		free(opcion_upper);
		return;
	}

	if (tp_quitar_obstaculo(menu->tp, JUGADOR_1, (unsigned int)posicion) ==
	    0) {
		mover_a_posicion(x, ++(*y));
		pantalla_verificar_y_limpiar(
			y, limite_y, x, menu_pantalla_juego_agregar_obstaculo);
		printf(ROJO NEGRITA
		       "   THAT POSITION CAN NOT BE REMOVED OR YOUR TRACK IS EMPTY." COLOR_RESET);

	} else {
		mover_a_posicion(x, ++(*y));
		pantalla_verificar_y_limpiar(
			y, limite_y, x, menu_pantalla_juego_agregar_obstaculo);
		printf(VERDE NEGRITA
		       "   OBSTACLE CORRECTLY REMOVED." COLOR_RESET);
	}

	free(opcion_upper);
}

static void usuario_elige_tipo_obstaculo(menu_t *menu, int x, int y)
{
	char opcion[10];
	int limite_y = 21;

	while (true) {
		mover_a_posicion(x, ++y);
		printf(NEGRITA "-> CHOOSE " AZUL "[F/D/I/R/B]: " COLOR_RESET);

		if (fgets(opcion, sizeof(opcion), stdin) != NULL) {
			opcion[strcspn(opcion, "\n")] = 0;
			char *opcion_upper = string_to_upper(opcion);
			if (!opcion_upper)
				return;

			if (strcmp(opcion_upper, "F") == 0 ||
			    strcmp(opcion_upper, "D") == 0 ||
			    strcmp(opcion_upper, "I") == 0) {
				obstaculos_opcion_agregar(menu, opcion_upper, x,
							  &y, limite_y);

			} else if (strcmp(opcion_upper, "R") == 0) {
				obstaculos_opcion_quitar(menu, opcion_upper, x,
							 &y, limite_y);

			} else if (strcmp(opcion_upper, "B") == 0) {
				free(opcion_upper);
				borrar_pantalla();
				menu_interactivo_pantalla_juego(menu);
				return;

			} else {
				free(opcion_upper);
				mover_a_posicion(x, ++y);
				pantalla_verificar_y_limpiar(
					&y, limite_y, x,
					menu_pantalla_juego_agregar_obstaculo);
				printf(ROJO NEGRITA
				       "   INVALID OPTION. PLEASE ENTER 'F', 'D', 'I' OR 'B'." COLOR_RESET);
			}
		}
	}
}

static int contador_dificultad(menu_t *menu)
{
	switch (menu->dificultad) {
	case EASY:
		return 5;

	case MEDIUM:
		return 4;

	case HARD:
		return 3;

	case IMPOSSIBLE:
		return 2;

	default:
		return ERROR;
	}
}

static void usuario_elige_pantalla_juego_carrera(menu_t *menu, int x, int y)
{
	char opcion[10];
	bool respuesta_valida = false;
	int limite_y = 21;

	static int intentos = ERROR;
	if (intentos == ERROR) {
		intentos = contador_dificultad(menu);
		if (intentos == ERROR)
			return;
	}

	while (!respuesta_valida) {
		mover_a_posicion(x, ++y);
		printf(NEGRITA ROJO "%d ATTEMPS REMAINING." COLOR_RESET,
		       intentos);

		mover_a_posicion(x, ++y);
		printf(NEGRITA "-> CHOOSE " AZUL "[F/R]: " COLOR_RESET);

		if (fgets(opcion, sizeof(opcion), stdin) != NULL) {
			opcion[strcspn(opcion, "\n")] = 0;
			char *opcion_upper = string_to_upper(opcion);
			if (!opcion_upper)
				return;

			if (strcmp(opcion_upper, "R") == 0) {
				if (intentos == 0)
					borrar_pantalla();
				else {
					intentos--;
					menu_interactivo_pantalla_juego(menu);
				}
				respuesta_valida = true;

			} else if (strcmp(opcion_upper, "F") == 0) {
				borrar_pantalla();
				respuesta_valida = true;

			} else {
				mover_a_posicion(x, ++y);
				juego_verificar_y_limpiar_pantalla(
					menu, &y, limite_y, x,
					menu_pantalla_juego_carrera);
				printf(ROJO NEGRITA
				       "   INVALID OPTION. PLEASE ENTER 'F', OR 'R'." COLOR_RESET);
			}
			free(opcion_upper);
		}
	}

	return;
}

static void juego_ejecutar_opcion(char opcion, menu_t *menu)
{
	switch (opcion) {
	case 'N':
		menu_interactivo_pantalla_juego_agregar_obstaculo(menu);
		break;

	case 'R':
		menu_interactivo_pantalla_juego_carrera(menu);
		break;

	case 'O':
		menu_interactivo_pantalla_juego_informacion_oponente(menu);
		break;

	case 'I':
		menu_interactivo_pantalla_pista_jugador(menu);
		break;
	}
	borrar_pantalla();
}

static void usuario_elige_pantalla_juego(menu_t *menu, int x, int y)
{
	char opcion[10];
	bool respuesta_valida = false;
	int limite_y = 21;

	while (!respuesta_valida) {
		mover_a_posicion(x, ++y);
		printf(NEGRITA "-> CHOOSE " AZUL "[N/R/O/I]: " COLOR_RESET);

		if (fgets(opcion, sizeof(opcion), stdin) != NULL) {
			opcion[strcspn(opcion, "\n")] = 0;
			char *opcion_upper = string_to_upper(opcion);
			if (!opcion_upper) {
				return;
			}

			if (strcmp(opcion_upper, "N") == 0 ||
			    strcmp(opcion_upper, "R") == 0 ||
			    strcmp(opcion_upper, "O") == 0 ||
			    strcmp(opcion_upper, "I") == 0) {
				juego_ejecutar_opcion(opcion_upper[0], menu);
				respuesta_valida = true;

			} else {
				mover_a_posicion(x, ++y);
				juego_verificar_y_limpiar_pantalla(
					menu, &y, limite_y, x,
					menu_pantalla_juego);
				printf(ROJO NEGRITA
				       "   INVALID OPTION. PLEASE ENTER 'N', 'R', 'O' OR 'I'." COLOR_RESET);
			}

			free(opcion_upper);
		}
	}
}

// ----------------------------------------------- Menu Crear y Destruir -----------------------------------------------

menu_t *menu_crear(const char *nombre_archivo)
{
	menu_t *menu = calloc(1, sizeof(menu_t));
	if (!menu)
		return NULL;

	menu->tp = tp_crear(nombre_archivo);
	if (!menu->tp) {
		free(menu);
		return NULL;
	}

	return menu;
}

void menu_destruir(menu_t *menu)
{
	if (!menu)
		return;

	tp_destruir(menu->tp);
	free(menu);
}

// ------------------------------------------- Menus Interactivos Secundarios -------------------------------------------

void menu_interactivo_pantalla_pista_jugador(menu_t *menu)
{
	menu_pantalla_pista_jugador(menu);
	esperar_usuario();
	borrar_pantalla();
	menu_interactivo_pantalla_juego(menu);
}

void menu_interactivo_pantalla_juego_informacion_oponente(menu_t *menu)
{
	menu_pantalla_juego_informacion_oponente(menu);
	esperar_usuario();
	borrar_pantalla();
	menu_interactivo_pantalla_juego(menu);
}

void menu_interactivo_pantalla_juego_agregar_obstaculo(menu_t *menu)
{
	menu_pantalla_juego_agregar_obstaculo();
	usuario_elige_tipo_obstaculo(menu, 9, 20);
}

void menu_interactivo_pantalla_juego_carrera(menu_t *menu)
{
	menu_pantalla_juego_carrera(menu);
	usuario_elige_pantalla_juego_carrera(menu, 9, 21);
}

// ------------------------------------------- Menus Interactivos Principales -------------------------------------------

void menu_interactivo_pantalla_inicial()
{
	menu_pantalla_inicial();
	esperar_usuario();
	borrar_pantalla();
}

int menu_interactivo_pantalla_principal()
{
	menu_pantalla_principal();
	return usuario_elige_main_menu(10, 20);
}

void menu_interactivo_pantalla_cambiar_archivo()
{
	menu_pantalla_cambiar_archivo();
	esperar_usuario();
	borrar_pantalla();
}

void menu_interactivo_pantalla_seleccion_dificultad(menu_t *menu)
{
	menu_pantalla_dificultad();
	menu->dificultad = usuario_elige_dificultad(9, 20);
	borrar_pantalla();
}

void menu_interactivo_pantalla_seleccion_pokemon_maquina(menu_t *menu)
{
	menu_pantalla_seleccion_pokemon_maquina(menu);
	esperar_usuario();
	borrar_pantalla();
}

void menu_interactivo_pantalla_pista_pokemon_maquina(menu_t *menu)
{
	menu_pantalla_pista_pokemon_maquina(menu);
	esperar_usuario();
	borrar_pantalla();
}

void menu_interactivo_pantalla_seleccion_pokemon_usuario(menu_t *menu)
{
	menu_pantalla_seleccion_pokemon_usuario(menu);
	usuario_selecciona_pokemon(menu, 6, 19);
	borrar_pantalla();
}

void menu_interactivo_pantalla_juego(menu_t *menu)
{
	menu_pantalla_juego(menu);
	usuario_elige_pantalla_juego(menu, 9, 21);
	borrar_pantalla();
}
