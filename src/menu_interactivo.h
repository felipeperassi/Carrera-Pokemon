#ifndef __MENU_INTERACTIVO_H__
#define __MENU_INTERACTIVO_H__

typedef struct menu menu_t;

menu_t *menu_crear(const char *nombre_archivo);

void menu_interactivo_pantalla_inicial();

int menu_interactivo_pantalla_principal();

void menu_interactivo_pantalla_cambiar_archivo();

void menu_interactivo_pantalla_seleccion_dificultad(menu_t *menu);

void menu_interactivo_pantalla_seleccion_pokemon_maquina(menu_t *menu);

void menu_interactivo_pantalla_pista_pokemon_maquina(menu_t *menu);

void menu_interactivo_pantalla_seleccion_pokemon_usuario(menu_t *menu);

void menu_interactivo_pantalla_juego(menu_t *menu);

void menu_interactivo_pantalla_pista_jugador(menu_t *menu);

void menu_interactivo_pantalla_juego_informacion_oponente(menu_t *menu);

void menu_interactivo_pantalla_juego_agregar_obstaculo(menu_t *menu);

void menu_interactivo_pantalla_juego_carrera(menu_t *menu);

void menu_destruir(menu_t *menu);

#endif /* __MENU_INTERACTIVO_H__ */