#include "menu.h"
#include "split.h"
#include <time.h>
#include <math.h>

// ----------------------------- Funciones Auxiliares para el manejo y printeo en pantalla ------------------------------

void borrar_pantalla()
{
	printf("\033[H\033[J");
}

void mover_a_posicion(int x, int y)
{
	printf("\033[%d;%dH", y, x);
}

static void printear_txt(const char *filename, int x, int y, const char *color,
			 const char *formatear)
{
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		perror("Error al abrir el archivo");
		return;
	}

	char line[256];
	mover_a_posicion(x, y);
	while (fgets(line, sizeof(line), file)) {
		printf("%s%s%s%s", color, formatear, line, COLOR_RESET);
		y++;
		mover_a_posicion(x, y);
	}

	fclose(file);
}

static void printear_cuadro(int x, int y, int ancho, int alto)
{
	mover_a_posicion(x, y);
	printf(NEGRITA "+");
	for (int i = 0; i < ancho - 2; i++) {
		printf("-");
	}
	printf("+");

	for (int i = 1; i < alto - 1; i++) {
		mover_a_posicion(x, y + i);
		printf("|");
		mover_a_posicion(x + ancho - 1, y + i);
		printf("|");
	}

	mover_a_posicion(x, y + alto - 1);
	printf("+");
	for (int i = 0; i < ancho - 2; i++) {
		printf("-");
	}
	printf("+" COLOR_RESET);
}

static char *pokemon_color_parametro(int parametro)
{
	static char color[10] = "\0";

	if (parametro > 6)
		strcpy(color, VERDE);

	else if (parametro < 4)
		strcpy(color, ROJO);

	else
		strcpy(color, AMARILLO);

	return color;
}

static char **separar_string(char *string, char delimitador)
{
	if (!string)
		return NULL;

	char **vector_nombres = split(string, delimitador);
	if (!vector_nombres) {
		free(string);
		return NULL;
	}

	return vector_nombres;
}

static void liberar_memoria(char **vector_strings, char *string, int cantidad)
{
	for (int i = 0; i < cantidad; i++)
		free(vector_strings[i]);

	free(vector_strings);
	free(string);
}

void pokemones_printear_disponibles(struct menu *menu, int x, int y)
{
	if (!menu)
		return;

	char *nombres_string = tp_nombres_disponibles(menu->tp);
	char **vector_nombres = separar_string(nombres_string, ',');
	if (!vector_nombres) {
		free(nombres_string);
		return;
	}

	int contador = 0;

	mover_a_posicion(x, y);
	for (int i = 0; i < tp_cantidad_pokemon(menu->tp); i++) {
		if (strcmp(vector_nombres[i],
			   tp_pokemon_seleccionado(menu->tp, JUGADOR_2)
				   ->nombre) == 0)
			printf(ROJO "%-15s" COLOR_RESET, vector_nombres[i]);

		else
			printf(VERDE "%-15s" COLOR_RESET, vector_nombres[i]);

		contador++;

		if (contador % 5 == 0) {
			y += 2;
			mover_a_posicion(x, y);
		}
	}

	if (contador % 5 != 0)
		mover_a_posicion(x, y + 2);

	liberar_memoria(vector_nombres, nombres_string,
			tp_cantidad_pokemon(menu->tp));
}

static void pokemon_printear_parametros(struct menu *menu,
					enum TP_JUGADOR jugador)
{
	const struct pokemon_info *pokemon =
		tp_pokemon_seleccionado(menu->tp, jugador);
	if (!pokemon)
		return;

	printf(NEGRITA "-> PARAMETERS: " COLOR_RESET);
	printf("%sSTRENGTH: %d" COLOR_RESET,
	       pokemon_color_parametro(pokemon->fuerza), pokemon->fuerza);
	printf(",%s SKILL: %d" COLOR_RESET,
	       pokemon_color_parametro(pokemon->destreza), pokemon->destreza);
	printf(",%s INTELLIGENCE: %d" COLOR_RESET,
	       pokemon_color_parametro(pokemon->inteligencia),
	       pokemon->inteligencia);
}

void pokemon_printear_seleccionado(struct menu *menu, enum TP_JUGADOR jugador,
				   int x, int *y)
{
	mover_a_posicion(x, ++(*y));
	printf(NEGRITA "-> SELECTED POKEMON: " AZUL "%s" COLOR_RESET,
	       tp_pokemon_seleccionado(menu->tp, jugador)->nombre);
	mover_a_posicion(x, ++(*y));
	pokemon_printear_parametros(menu, jugador);
}

static void pista_printear(struct menu *menu, enum TP_JUGADOR jugador,
			   int separacion, int x, int y, int alineado_pista)
{
	char *pista = tp_obstaculos_pista(menu->tp, jugador);
	if (!pista)
		return;

	mover_a_posicion(x, y);
	printf(ROJO "-> HERE YOU HAVE SOME INFO..." COLOR_RESET);
	y += 2;
	x += alineado_pista;

	int columnas = 3;
	int contador = 0;

	for (int i = 0; i < strlen(pista); i += separacion) {
		if (jugador == JUGADOR_1)
			mover_a_posicion(x + (contador % columnas) * 25,
					 y + (contador / columnas));

		else {
			mover_a_posicion(x, y);
			y += 2;
		}

		printf(NEGRITA BLANCO "TYPE: " AZUL "%c" BLANCO
				      ", POSITION: " AZUL "%d\n" COLOR_RESET,
		       pista[i], i + 1);
		contador++;
	}

	free(pista);
}

static void maquina_printear_obstaculos_insertados(struct menu *menu, int x,
						   int y)
{
	mover_a_posicion(x, y);
	switch (menu->dificultad) {
	case EASY:
		printf(ROJO
		       "-> 4 OBSTACLES WERE INSERTED BY YOUR OPPONENT." COLOR_RESET);
		pista_printear(menu, JUGADOR_2, 1, x, y + 1, 14);
		break;

	case MEDIUM:
		printf(ROJO
		       "-> 7 OBSTACLES WERE INSERTED BY YOUR OPPONENT." COLOR_RESET);
		pista_printear(menu, JUGADOR_2, 2, x, y + 1, 14);
		break;

	case HARD:
		printf(ROJO
		       "-> 10 OBSTACLES WERE INSERTED BY YOUR OPPONENT." COLOR_RESET);
		pista_printear(menu, JUGADOR_2, 3, x, y + 1, 14);
		break;

	case IMPOSSIBLE:
		printf(ROJO
		       "-> 15 OBSTACLES WERE INSERTED BY YOUR OPPONENT." COLOR_RESET);
		pista_printear(menu, JUGADOR_2, 5, x, y + 1, 14);
		break;
	}

	return;
}

// --------------------------------- Funciones Auxiliares para implementar lógicas -----------------------------------

static void maquina_seleccionar_pokemon(struct menu *menu)
{
	if (!menu)
		return;

	srand((unsigned int)time(NULL));

	int numero_pokemon = rand() % tp_cantidad_pokemon(menu->tp);

	char *nombres_string = tp_nombres_disponibles(menu->tp);
	char **vector_nombres = separar_string(nombres_string, ',');
	if (!vector_nombres) {
		free(nombres_string);
		return;
	}

	if (tp_seleccionar_pokemon(menu->tp, JUGADOR_2,
				   vector_nombres[numero_pokemon]) == false) {
		liberar_memoria(vector_nombres, nombres_string,
				tp_cantidad_pokemon(menu->tp));
		return;
	}

	liberar_memoria(vector_nombres, nombres_string,
			tp_cantidad_pokemon(menu->tp));
}

static void maquina_obstaculos_insertar_n(struct menu *menu, int cantidad)
{
	if (!menu->tp)
		return;

	srand((unsigned int)time(NULL));

	for (int i = 0; i < cantidad; i++) {
		unsigned int obstaculo_random = (unsigned int)(rand() % 3);
		unsigned int posicion_random = (unsigned int)(rand() % (i + 1));

		if (tp_agregar_obstaculo(menu->tp, JUGADOR_2, obstaculo_random,
					 posicion_random) == 0)
			return;
	}
}

static void maquina_obstaculos_insertar(struct menu *menu)
{
	switch (menu->dificultad) {
	case EASY:
		maquina_obstaculos_insertar_n(menu, 4);
		break;

	case MEDIUM:
		maquina_obstaculos_insertar_n(menu, 7);
		break;

	case HARD:
		maquina_obstaculos_insertar_n(menu, 10);
		break;

	case IMPOSSIBLE:
		maquina_obstaculos_insertar_n(menu, 15);
		break;
	}

	return;
}

static double juego_calcular_score(int tiempo_jugador1, int tiempo_jugador2)
{
	mover_a_posicion(30, 24);

	double modulo = fabs(tiempo_jugador1 - tiempo_jugador2);
	double suma = (double)tiempo_jugador1 + (double)tiempo_jugador2;

	double puntaje_final = (double)100 - ((double)100 * (modulo / suma));
	if (puntaje_final < 0)
		puntaje_final = 0;

	return puntaje_final;
}

// ----------------------------------------- Printeo de los Menus Secundarios -----------------------------------------

void menu_pantalla_pista_jugador(struct menu *menu)
{
	borrar_pantalla();

	printear_txt("dibujos/play.txt", 23, 1, NORMAL, NEGRITA);

	mover_a_posicion(2, 9);
	printf(VERDE "PLAYER 1 (YOU):" COLOR_RESET);
	int x1 = 17;
	int y1 = 8;
	pokemon_printear_seleccionado(menu, JUGADOR_1, x1, &y1);

	mover_a_posicion(16, 12);
	printf(ROJO "-> THESE OBSTACLES WERE INSERTED BY YOU." COLOR_RESET);

	pista_printear(menu, JUGADOR_1, 1, 16, 13, -10);

	mover_a_posicion(26, 23);
	printf(AMARILLO NEGRITA "PRESS " VERDE "'ENTER' " AMARILLO
				"TO GO BACK... " COLOR_RESET);
}

void menu_pantalla_juego_informacion_oponente(struct menu *menu)
{
	borrar_pantalla();

	printear_txt("dibujos/play.txt", 23, 1, NORMAL, NEGRITA);

	mover_a_posicion(2, 9);
	printf(ROJO "PLAYER 2 (YOUR OPPONENT):" COLOR_RESET);
	int x2 = 27;
	int y2 = 8;
	pokemon_printear_seleccionado(menu, JUGADOR_2, x2, &y2);

	maquina_printear_obstaculos_insertados(menu, 14, 12);

	mover_a_posicion(26, 23);
	printf(AMARILLO NEGRITA "PRESS " VERDE "'ENTER' " AMARILLO
				"TO GO BACK... " COLOR_RESET);
}

void menu_pantalla_juego_agregar_obstaculo()
{
	borrar_pantalla();

	printear_txt("dibujos/build.txt", 20, 1, NORMAL, NEGRITA);

	mover_a_posicion(6, 8);
	printf(AMARILLO
	       "CHOOSE THE TYPE AND THE POSITION OF THE OBSTACLE THAT YOU WANT TO ADD..." COLOR_RESET);

	mover_a_posicion(5, 9);
	printf(AMARILLO
	       "(PS: IF THE POSITION EXCEEDS THE TOTAL, THE OBSTACLE IS PLACED AT THE END)" COLOR_RESET);

	printear_cuadro(20, 11, 40, 8);

	mover_a_posicion(25, 12);
	printf(AMARILLO NEGRITA "PRESS " VERDE "'F' " AMARILLO
				"FOR STRENGTH OBSTACLE" COLOR_RESET);

	mover_a_posicion(26, 13);
	printf(AMARILLO NEGRITA "PRESS " VERDE "'D' " AMARILLO
				"FOR SKILL OBSTACLE" COLOR_RESET);

	mover_a_posicion(23, 14);
	printf(AMARILLO NEGRITA "PRESS " VERDE "'I' " AMARILLO
				"FOR INTELLIGENCE OBSTACLE" COLOR_RESET);

	mover_a_posicion(26, 15);
	printf(AMARILLO NEGRITA "PRESS " VERDE "'R' " AMARILLO
				"TO REMOVE OBSTACLE" COLOR_RESET);

	mover_a_posicion(30, 17);
	printf(ROJO NEGRITA "PRESS 'B' TO GO BACK" COLOR_RESET);
}

void menu_pantalla_juego_carrera(struct menu *menu)
{
	borrar_pantalla();

	printear_txt("dibujos/score.txt", 3, 1, NORMAL, NEGRITA);

	int tiempo_jugador1 =
		(int)tp_calcular_tiempo_pista(menu->tp, JUGADOR_1);
	int tiempo_jugador2 =
		(int)tp_calcular_tiempo_pista(menu->tp, JUGADOR_2);

	printear_cuadro(8, 8, 26, 5);
	mover_a_posicion(14, 10);
	printf(VERDE "YOUR TIME: %d" COLOR_RESET, tiempo_jugador1);

	mover_a_posicion(39, 10);
	printf(AZUL "VS" COLOR_RESET);

	printear_cuadro(45, 8, 30, 5);
	mover_a_posicion(48, 10);
	printf(ROJO "YOUR OPPONENT'S TIME: %d" COLOR_RESET, tiempo_jugador2);

	double score = juego_calcular_score(tiempo_jugador1, tiempo_jugador2);

	mover_a_posicion(30, 15);
	if (score > 90) {
		printf(NEGRITA "-> YOUR SCORE: " VERDE "%.1f" COLOR_RESET,
		       score);

		mover_a_posicion(33, 17);
		printf(NEGRITA VERDE "OUTSTANDING! :)" COLOR_RESET);

	} else if (score > 75 && score < 90) {
		printf(NEGRITA "-> YOUR SCORE: " AMARILLO "%.1f" COLOR_RESET,
		       score);

		mover_a_posicion(35, 17);
		printf(NEGRITA AMARILLO "NICE! :/" COLOR_RESET);

	} else {
		printf(NEGRITA "-> YOUR SCORE: " ROJO "%.1f" COLOR_RESET,
		       score);
		mover_a_posicion(33, 17);
		printf(ROJO NEGRITA "KEEP TRYING! :(" COLOR_RESET);
	}

	mover_a_posicion(20, 19);
	printf(NEGRITA AMARILLO "PRESS " VERDE "'F' " AMARILLO
				"TO FINISH GAME OR " ROJO "'R' " AMARILLO
				"TO GO BACK..." COLOR_RESET);
}

// ----------------------------------------- Printeo de los Menus Principales -----------------------------------------

void menu_pantalla_inicial()
{
	borrar_pantalla();

	printear_txt("dibujos/logo_pokemon.txt", 8, 3, NORMAL, NEGRITA);
	printear_txt("dibujos/race_letras.txt", 25, 14, AZUL, NEGRITA);

	mover_a_posicion(31, 22);
	printf(AMARILLO NEGRITA "PRESS " VERDE "'ENTER' " AMARILLO
				"TO START... " COLOR_RESET);
}

void menu_pantalla_principal()
{
	borrar_pantalla();

	printear_txt("dibujos/main_menu.txt", 8, 1, NORMAL, NEGRITA);
	printear_txt("dibujos/saludo.txt", 46, 10, AZUL, FORMATO_NORMAL);

	mover_a_posicion(10, 12);
	printf(AMARILLO NEGRITA "PRESS " VERDE "'1' " AMARILLO
				"TO START GAME" COLOR_RESET);

	mover_a_posicion(10, 16);
	printf(AMARILLO NEGRITA "PRESS " VERDE "'2' " AMARILLO
				"TO CHANGE POKEMON'S FILE" COLOR_RESET);
}

void menu_pantalla_cambiar_archivo()
{
	borrar_pantalla();

	printear_txt("dibujos/file.txt", 25, 1, NORMAL, NEGRITA);
	printear_txt("dibujos/pikachu.txt", 2, 10, AMARILLO, FORMATO_NORMAL);

	mover_a_posicion(10, 8);
	printf(AMARILLO NEGRITA
	       "FOLLOW THE NEXT INSTRUCTIONS TO UPLOAD YOUR OWN POKEMONS:" COLOR_RESET);

	mover_a_posicion(12, 9);
	printf(ROJO NEGRITA "REMEMBER: " COLOR_RESET
			    "THE MAX AMOUNT OF POKEMONS SUPPORTED IS 25");

	mover_a_posicion(30, 12);
	printf(AZUL NEGRITA
	       "1. " COLOR_RESET NEGRITA
	       "CREATE A .TXT FILE WITH THE FOLLOWING FORMAT:" COLOR_RESET);
	mover_a_posicion(35, 14);
	printf(AZUL "-> NAME,STRENGTH,SKILL,INTELLIGENCE" COLOR_RESET);

	mover_a_posicion(30, 16);
	printf(AZUL NEGRITA
	       "2. " COLOR_RESET NEGRITA
	       "UPLOAD IT TO THE FOLDER NAMED 'EJEMPLO'." COLOR_RESET);

	mover_a_posicion(30, 18);
	printf(AZUL NEGRITA
	       "3. " COLOR_RESET NEGRITA
	       "COMPILE AND RUN WITH THE FOLLOWING LINE:" COLOR_RESET);
	mover_a_posicion(40, 20);
	printf(AZUL "-> ./JUEGO <FILE NAME>" COLOR_RESET);

	mover_a_posicion(41, 23);
	printf(AMARILLO NEGRITA "PRESS ENTER TO FINISH... " COLOR_RESET);
}

void menu_pantalla_dificultad()
{
	borrar_pantalla();

	printear_txt("dibujos/difficulty.txt", 7, 1, NORMAL, NEGRITA);
	printear_txt("dibujos/susto.txt", 50, 7, AMARILLO, FORMATO_NORMAL);

	mover_a_posicion(9, 9);
	printf(AMARILLO NEGRITA "SELECT DIFFICULTY LEVEL:" COLOR_RESET);

	printear_cuadro(18, 11, 18, 9);

	mover_a_posicion(25, 12);
	printf(VERDE NEGRITA "EASY" COLOR_RESET);

	mover_a_posicion(24, 14);
	printf(AZUL NEGRITA "MEDIUM" COLOR_RESET);

	mover_a_posicion(25, 16);
	printf(AMARILLO NEGRITA "HARD" COLOR_RESET);

	mover_a_posicion(22, 18);
	printf(ROJO NEGRITA "IMPOSSIBLE" COLOR_RESET);
}

void menu_pantalla_seleccion_pokemon_maquina(struct menu *menu)
{
	borrar_pantalla();

	printear_txt("dibujos/competitor.txt", 5, 1, NORMAL, NEGRITA);
	printear_txt("dibujos/pokeball.txt", 5, 7, ROJO, FORMATO_NORMAL);

	mover_a_posicion(35, 8);
	printf(AMARILLO NEGRITA "YOUR OPPONENT'S POKEMON IS..." COLOR_RESET);

	maquina_seleccionar_pokemon(menu);
	int x = 15;
	int y = 15;
	pokemon_printear_seleccionado(menu, JUGADOR_2, x, &y);

	mover_a_posicion(37, 22);
	printf(AMARILLO NEGRITA "PRESS " VERDE "'ENTER' " AMARILLO
				"TO CONTINUE... " COLOR_RESET);
}

void menu_pantalla_pista_pokemon_maquina(struct menu *menu)
{
	borrar_pantalla();

	printear_txt("dibujos/his_track.txt", 8, 1, NORMAL, NEGRITA);
	printear_txt("dibujos/flag.txt", 2, 8, NORMAL, FORMATO_NORMAL);

	mover_a_posicion(30, 8);
	printf(AMARILLO NEGRITA
	       "YOUR OPPONENT'S TRACK IS ALREADY CREATED..." COLOR_RESET);

	mover_a_posicion(24, 9);
	printf(AMARILLO NEGRITA
	       "(PS: F FOR STRENGTH, D FOR SKILL AND I FOR INTELLIGENCE)" COLOR_RESET);

	maquina_obstaculos_insertar(menu);
	maquina_printear_obstaculos_insertados(menu, 30, 12);

	mover_a_posicion(43, 23);
	printf(AMARILLO NEGRITA "PRESS " VERDE "'ENTER' " AMARILLO
				"TO CONTINUE... " COLOR_RESET);
}

void menu_pantalla_seleccion_pokemon_usuario(struct menu *menu)
{
	borrar_pantalla();

	printear_txt("dibujos/select.txt", 18, 1, NORMAL, NEGRITA);

	mover_a_posicion(15, 8);
	printf(AMARILLO NEGRITA
	       "CHOOSE YOUR POKEMON (GREENS ARE THE ONES AVAILABLE):" COLOR_RESET);

	pokemones_printear_disponibles(menu, 6, 10);
}

void menu_pantalla_juego(struct menu *menu)
{
	borrar_pantalla();
	printear_txt("dibujos/play.txt", 23, 1, NORMAL, NEGRITA);

	mover_a_posicion(2, 8);
	printf(VERDE "PLAYER 1 (YOU):" COLOR_RESET);

	int x1 = 17;
	int y1 = 7;
	pokemon_printear_seleccionado(menu, JUGADOR_1, x1, &y1);

	mover_a_posicion(2, 11);
	printf(ROJO "PLAYER 2 (YOUR OPPONENT):" COLOR_RESET);
	int x2 = 27;
	int y2 = 10;
	pokemon_printear_seleccionado(menu, JUGADOR_2, x2, &y2);

	printear_cuadro(20, 14, 40, 6);
	mover_a_posicion(26, 15);
	printf(AMARILLO NEGRITA "PRESS " VERDE "'N' " AMARILLO
				"TO BUILD YOUR TRACK" COLOR_RESET);

	mover_a_posicion(29, 16);
	printf(AMARILLO NEGRITA "PRESS " VERDE "'R' " AMARILLO
				"TO START RACE" COLOR_RESET);

	mover_a_posicion(24, 17);
	printf(AMARILLO NEGRITA "PRESS " VERDE "'O' " AMARILLO
				"TO SEE OPPONENT'S INFO" COLOR_RESET);

	mover_a_posicion(27, 18);
	printf(AMARILLO NEGRITA "PRESS " VERDE "'I' " AMARILLO
				"TO SEE YOUR INFO" COLOR_RESET);
}