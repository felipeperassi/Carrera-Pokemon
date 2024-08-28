#include "lista.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct nodo {
	void *elemento;
	struct nodo *siguiente;
} nodo_t;

struct lista {
	nodo_t *nodo_inicial;
	nodo_t *nodo_final;
	size_t tamanio;
};

struct lista_iterador {
	lista_t *lista;
	nodo_t *nodo_actual;
};

static nodo_t *nodo_crear(nodo_t *nodo_siguiente, void *elemento)
{
	nodo_t *nuevo_nodo = calloc(1, sizeof(nodo_t));
	if (!nuevo_nodo)
		return NULL;

	nuevo_nodo->siguiente = nodo_siguiente;
	nuevo_nodo->elemento = elemento;

	return nuevo_nodo;
}

lista_t *lista_crear()
{
	return calloc(1, sizeof(lista_t));
}

lista_t *lista_insertar(lista_t *lista, void *elemento)
{
	if (!lista)
		return NULL;

	nodo_t *nuevo_nodo = nodo_crear(NULL, elemento);
	if (!nuevo_nodo)
		return NULL;

	if (lista_vacia(lista))
		lista->nodo_inicial = nuevo_nodo;
	else
		lista->nodo_final->siguiente = nuevo_nodo;

	lista->nodo_final = nuevo_nodo;
	lista->tamanio++;
	return lista;
}

lista_t *lista_insertar_en_posicion(lista_t *lista, void *elemento,
				    size_t posicion)
{
	if (!lista)
		return NULL;

	nodo_t *nuevo_nodo = nodo_crear(NULL, elemento);
	if (!nuevo_nodo)
		return NULL;

	if (lista_tamanio(lista) <= posicion) {
		if (lista_vacia(lista))
			lista->nodo_inicial = nuevo_nodo;
		else
			lista->nodo_final->siguiente = nuevo_nodo;

		lista->nodo_final = nuevo_nodo;

	} else if (posicion == 0) {
		nuevo_nodo->siguiente = lista->nodo_inicial;
		lista->nodo_inicial = nuevo_nodo;

	} else {
		nodo_t *nodo_actual = lista->nodo_inicial;
		for (size_t i = 0; i < posicion - 1; i++)
			nodo_actual = nodo_actual->siguiente;

		nuevo_nodo->siguiente = nodo_actual->siguiente;
		nodo_actual->siguiente = nuevo_nodo;
	}

	lista->tamanio++;
	return lista;
}

void *lista_quitar(lista_t *lista)
{
	if (!lista || lista_vacia(lista))
		return NULL;

	nodo_t *nodo_actual = lista->nodo_inicial;
	void *elemento_extraido = NULL;

	if (lista_tamanio(lista) == 1) {
		elemento_extraido = nodo_actual->elemento;

		free(nodo_actual);
		lista->nodo_final = NULL;
		lista->nodo_inicial = NULL;

	} else {
		for (size_t i = 2; i < lista_tamanio(lista); i++)
			nodo_actual = nodo_actual->siguiente;

		elemento_extraido = nodo_actual->siguiente->elemento;

		free(nodo_actual->siguiente);
		lista->nodo_final = nodo_actual;
		nodo_actual->siguiente = NULL;
	}

	lista->tamanio--;
	return elemento_extraido;
}

void *lista_quitar_de_posicion(lista_t *lista, size_t posicion)
{
	if (!lista || lista_vacia(lista))
		return NULL;

	nodo_t *nodo_a_quitar = NULL;
	void *elemento_extraido = NULL;

	if (posicion >= lista->tamanio - 1) {
		return lista_quitar(lista);
	}

	if (posicion == 0) {
		nodo_a_quitar = lista->nodo_inicial;
		lista->nodo_inicial = lista->nodo_inicial->siguiente;
		if (!lista->nodo_inicial)
			lista->nodo_final = NULL;

	} else {
		nodo_t *nodo_anterior = lista->nodo_inicial;
		for (size_t i = 0; i < posicion - 1; i++)
			nodo_anterior = nodo_anterior->siguiente;

		nodo_a_quitar = nodo_anterior->siguiente;
		nodo_anterior->siguiente = nodo_a_quitar->siguiente;
		if (!nodo_anterior->siguiente)
			lista->nodo_final = nodo_anterior;
	}

	elemento_extraido = nodo_a_quitar->elemento;
	free(nodo_a_quitar);
	lista->tamanio--;

	return elemento_extraido;
}

void *lista_elemento_en_posicion(lista_t *lista, size_t posicion)
{
	if (!lista || lista_vacia(lista) || lista_tamanio(lista) <= posicion)
		return NULL;

	nodo_t *nodo_actual = lista->nodo_inicial;

	for (size_t i = 0; i < posicion; i++)
		nodo_actual = nodo_actual->siguiente;

	return (nodo_actual->elemento);
}

void *lista_buscar_elemento(lista_t *lista, int (*comparador)(void *, void *),
			    void *contexto)
{
	if (!lista || lista_vacia(lista) || !comparador)
		return NULL;

	nodo_t *nodo_actual = lista->nodo_inicial;
	void *elemento = NULL;
	bool encontrado = false;

	for (size_t i = 0; i < lista_tamanio(lista) && !encontrado; i++) {
		if (comparador(nodo_actual->elemento, contexto) == 0) {
			elemento = nodo_actual->elemento;
			encontrado = true;
		}

		nodo_actual = nodo_actual->siguiente;
	}

	return elemento;
}

void *lista_primero(lista_t *lista)
{
	return (!lista || lista_vacia(lista)) ? NULL :
						lista->nodo_inicial->elemento;
}

void *lista_ultimo(lista_t *lista)
{
	return (!lista || lista_vacia(lista)) ? NULL :
						lista->nodo_final->elemento;
}

bool lista_vacia(lista_t *lista)
{
	return (!lista || !lista->nodo_inicial || lista->tamanio == 0) ? true :
									 false;
}

size_t lista_tamanio(lista_t *lista)
{
	return !lista ? 0 : lista->tamanio;
}

void lista_destruir(lista_t *lista)
{
	lista_destruir_todo(lista, NULL);
}

void lista_destruir_todo(lista_t *lista, void (*funcion)(void *))
{
	if (!lista)
		return;

	nodo_t *nodo_actual = lista->nodo_inicial;
	nodo_t *nodo_siguiente = NULL;

	while (nodo_actual != NULL) {
		nodo_siguiente = nodo_actual->siguiente;
		if (funcion != NULL)
			funcion(nodo_actual->elemento);
		free(nodo_actual);
		nodo_actual = nodo_siguiente;
	}

	free(lista);
}

lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
	if (!lista)
		return NULL;

	lista_iterador_t *nuevo_iterador = calloc(1, sizeof(lista_iterador_t));
	if (!nuevo_iterador)
		return NULL;

	nuevo_iterador->lista = lista;
	nuevo_iterador->nodo_actual = lista->nodo_inicial;

	return nuevo_iterador;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t *iterador)
{
	return !iterador ? false : iterador->nodo_actual;
}

bool lista_iterador_avanzar(lista_iterador_t *iterador)
{
	return (!iterador || !iterador->nodo_actual) ?
		       false :
		       (iterador->nodo_actual =
				iterador->nodo_actual->siguiente);
}

void *lista_iterador_elemento_actual(lista_iterador_t *iterador)
{
	return (!iterador || !iterador->nodo_actual) ?
		       NULL :
		       iterador->nodo_actual->elemento;
}

void lista_iterador_destruir(lista_iterador_t *iterador)
{
	free(iterador);
}

size_t lista_con_cada_elemento(lista_t *lista, bool (*funcion)(void *, void *),
			       void *contexto)
{
	if (!lista || !funcion || lista_vacia(lista))
		return 0;

	nodo_t *nodo_actual = lista->nodo_inicial;
	size_t contador = 0;
	bool seguir = true;

	for (; contador < lista_tamanio(lista) && seguir; contador++) {
		seguir = funcion(nodo_actual->elemento, contexto);
		nodo_actual = nodo_actual->siguiente;
	}

	return contador;
}