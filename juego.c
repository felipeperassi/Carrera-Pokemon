#include "src/menu_interactivo.h"
#include <stdlib.h>
#include <stdio.h>

#define ERROR -1

#define COLOR_RESET "\033[0m"
#define ROJO "\x1b[31;1m"

int main(int argc, char const *argv[])
{
	if (!argv[1]) {
		printf(ROJO "THE FILE DOES NOT EXIST.\n" COLOR_RESET);
		return ERROR;
	}

	menu_t *menu = menu_crear(argv[1]);
	if (!menu) {
		printf(ROJO
		       "THE FILE DOES NOT HAVE THE SPECIFIED FORMAT. PLEASE RETRY.\n" COLOR_RESET);
		return ERROR;
	}

	menu_interactivo_pantalla_inicial();

	int numero_camino = menu_interactivo_pantalla_principal();
	if (numero_camino == ERROR) {
		menu_destruir(menu);
		return ERROR;
	}

	if (numero_camino == 2)
		menu_interactivo_pantalla_cambiar_archivo();

	if (numero_camino == 1) {
		menu_interactivo_pantalla_seleccion_dificultad(menu);
		menu_interactivo_pantalla_seleccion_pokemon_maquina(menu);
		menu_interactivo_pantalla_pista_pokemon_maquina(menu);
		menu_interactivo_pantalla_seleccion_pokemon_usuario(menu);
		menu_interactivo_pantalla_juego(menu);
	}

	menu_destruir(menu);

	return 0;
}
