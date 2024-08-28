#include <stdlib.h>
#include <string.h>

void liberar_memoria(char **vector_a_liberar, size_t tope)
{
	for (size_t i = 0; i < tope; i++) {
		free(vector_a_liberar[i]);
	}
	free(vector_a_liberar);
}

char **split(const char *string, char separador)
{
	if (string == NULL) {
		return NULL;
	}

	char **vector_palabras = NULL;
	char *palabra = NULL;
	size_t tope = 0;
	size_t inicio_palabra = 0;
	size_t longitud_palabra = 0;

	for (size_t i = 0; i <= strlen(string); i++) {
		if (string[i] == separador || string[i] == '\0') {
			vector_palabras = realloc(vector_palabras,
						  (tope + 1) * sizeof(char *));
			if (vector_palabras == NULL) {
				liberar_memoria(vector_palabras, tope);
				return NULL;
			}

			longitud_palabra = i - inicio_palabra;

			palabra = malloc((longitud_palabra + 1) * sizeof(char));
			if (palabra == NULL) {
				liberar_memoria(vector_palabras, tope);
				return NULL;
			}

			strncpy(palabra, string + inicio_palabra,
				longitud_palabra);
			palabra[longitud_palabra] = '\0';

			vector_palabras[tope] = palabra;
			inicio_palabra = i + 1;
			tope++;
		}
	}

	vector_palabras = realloc(vector_palabras, (tope + 1) * sizeof(char *));
	if (vector_palabras == NULL) {
		liberar_memoria(vector_palabras, tope);
		return NULL;
	}

	vector_palabras[tope] = NULL;

	return vector_palabras;
}