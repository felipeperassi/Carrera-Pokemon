#include "abb.h"
#include "abb_estructura_privada.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define REFERENCIA 0

//--------------------------------------------------- funciones nodo_abb ---------------------------------------------------

nodo_abb_t *nodo_crear(nodo_abb_t *nodo_izquierdo, nodo_abb_t *nodo_derecho,
		       void *elemento)
{
	nodo_abb_t *nuevo_nodo = calloc(1, sizeof(nodo_abb_t));
	if (!nuevo_nodo)
		return NULL;

	nuevo_nodo->izquierda = nodo_izquierdo;
	nuevo_nodo->derecha = nodo_derecho;
	nuevo_nodo->elemento = elemento;

	return nuevo_nodo;
}

nodo_abb_t *nodo_insertar(nodo_abb_t *nodo, abb_comparador comparador,
			  void *elemento)
{
	if (!comparador)
		return NULL;

	if (!nodo)
		nodo = nodo_crear(NULL, NULL, elemento);

	else if (comparador(elemento, nodo->elemento) >
		 REFERENCIA) // si es igual se va del lado derecho
		nodo->derecha =
			nodo_insertar(nodo->derecha, comparador, elemento);

	else
		nodo->izquierda =
			nodo_insertar(nodo->izquierda, comparador, elemento);

	return nodo;
}

nodo_abb_t *nodo_obtener_predecesor(nodo_abb_t *nodo)
{
	if (!nodo->derecha)
		return nodo;

	return nodo_obtener_predecesor(nodo->derecha);
}

nodo_abb_t *nodo_reemplazar_predecesor(nodo_abb_t *nodo,
				       nodo_abb_t *predecesor_hijo)
{
	if (!nodo->derecha)
		return predecesor_hijo;

	nodo->derecha =
		nodo_reemplazar_predecesor(nodo->derecha, predecesor_hijo);

	return nodo;
}

void nodo_quitar_hijo(nodo_abb_t **nodo)
{
	nodo_abb_t *nodo_borrar = *nodo;

	if (nodo_borrar->derecha && nodo_borrar->izquierda) {
		*nodo = nodo_obtener_predecesor(nodo_borrar->izquierda);
		(*nodo)->izquierda = nodo_reemplazar_predecesor(
			nodo_borrar->izquierda, (*nodo)->izquierda);
		(*nodo)->derecha = nodo_borrar->derecha;

	} else {
		*nodo = nodo_borrar->izquierda ? nodo_borrar->izquierda :
						 nodo_borrar->derecha;
	}

	free(nodo_borrar);
}

void nodo_quitar_hoja(nodo_abb_t **nodo)
{
	free(*nodo);
	*nodo = NULL;
}

void *nodo_quitar(nodo_abb_t **nodo, abb_comparador comparador, void *elemento,
		  bool *se_quito)
{
	if (!*nodo || !comparador)
		return NULL;

	int comparacion = comparador(elemento, (*nodo)->elemento);

	if (comparacion == REFERENCIA) {
		void *elemento_eliminar = (*nodo)->elemento;

		if ((*nodo)->derecha || (*nodo)->izquierda)
			nodo_quitar_hijo(nodo);
		else
			nodo_quitar_hoja(nodo);

		*se_quito = true;

		return elemento_eliminar;
	}

	if (comparacion > REFERENCIA)
		return nodo_quitar(&(*nodo)->derecha, comparador, elemento,
				   se_quito);

	return nodo_quitar(&(*nodo)->izquierda, comparador, elemento, se_quito);
}

void *nodo_buscar(nodo_abb_t *nodo, abb_comparador comparador, void *elemento)
{
	if (!nodo || !comparador)
		return NULL;

	int comparacion = comparador(elemento, nodo->elemento);

	if (comparacion == REFERENCIA)
		return nodo->elemento;

	return comparacion > REFERENCIA ?
		       nodo_buscar(nodo->derecha, comparador, elemento) :
		       nodo_buscar(nodo->izquierda, comparador, elemento);
}

void nodo_destruir(nodo_abb_t *nodo, void (*destructor)(void *))
{
	if (!nodo)
		return;

	nodo_destruir(nodo->izquierda, destructor);
	nodo_destruir(nodo->derecha, destructor);

	if (destructor)
		destructor(nodo->elemento);

	free(nodo);
}

bool nodo_con_cada_elemento_preorden(nodo_abb_t *nodo,
				     bool (*funcion)(void *, void *), void *aux,
				     size_t *contador)
{
	if (!nodo)
		return true;

	(*contador)++;

	if (!funcion(nodo->elemento, aux))
		return false;

	if (!nodo_con_cada_elemento_preorden(nodo->izquierda, funcion, aux,
					     contador) ||
	    !nodo_con_cada_elemento_preorden(nodo->derecha, funcion, aux,
					     contador))
		return false;

	return true;
}

bool nodo_con_cada_elemento_inorden(nodo_abb_t *nodo,
				    bool (*funcion)(void *, void *), void *aux,
				    size_t *contador)
{
	if (!nodo)
		return true;

	if (!nodo_con_cada_elemento_inorden(nodo->izquierda, funcion, aux,
					    contador))
		return false;

	(*contador)++;

	if (!funcion(nodo->elemento, aux))
		return false;

	if (!nodo_con_cada_elemento_inorden(nodo->derecha, funcion, aux,
					    contador))
		return false;

	return true;
}

bool nodo_con_cada_elemento_postorden(nodo_abb_t *nodo,
				      bool (*funcion)(void *, void *),
				      void *aux, size_t *contador)
{
	if (!nodo)
		return true;

	if (!nodo_con_cada_elemento_postorden(nodo->izquierda, funcion, aux,
					      contador) ||
	    !nodo_con_cada_elemento_postorden(nodo->derecha, funcion, aux,
					      contador))
		return false;

	(*contador)++;

	if (!funcion(nodo->elemento, aux))
		return false;

	return true;
}

typedef struct {
	void **array;
	size_t tamanio;
	size_t indice;
} recorrer_array_t;

bool copiar_array(void *elemento1, void *elemento2)
{
	recorrer_array_t *array = elemento2;

	array->array[array->indice] = elemento1;
	(array->indice)++;

	if (array->indice > array->tamanio - 1)
		return false;

	return true;
}

//----------------------------------------------------- funciones abb -----------------------------------------------------

abb_t *abb_crear(abb_comparador comparador)
{
	if (!comparador)
		return NULL;

	abb_t *arbol_nuevo = calloc(1, sizeof(abb_t));
	if (!arbol_nuevo)
		return NULL;

	arbol_nuevo->comparador = comparador;

	return arbol_nuevo;
}

abb_t *abb_insertar(abb_t *arbol, void *elemento)
{
	if (!arbol)
		return NULL;

	arbol->nodo_raiz =
		nodo_insertar(arbol->nodo_raiz, arbol->comparador, elemento);

	if (!arbol->nodo_raiz)
		return NULL;

	arbol->tamanio++;

	return arbol;
}

void *abb_quitar(abb_t *arbol, void *elemento)
{
	if (!arbol || abb_vacio(arbol))
		return NULL;

	bool se_quito = false;
	void *elemento_eliminado = nodo_quitar(
		&(arbol->nodo_raiz), arbol->comparador, elemento, &se_quito);

	if (se_quito)
		arbol->tamanio--;

	return elemento_eliminado;
}

void *abb_buscar(abb_t *arbol, void *elemento)
{
	return (!arbol || abb_vacio(arbol)) ?
		       NULL :
		       nodo_buscar(arbol->nodo_raiz, arbol->comparador,
				   elemento);
}

bool abb_vacio(abb_t *arbol)
{
	return (!arbol || !arbol->nodo_raiz || arbol->tamanio == 0);
}

size_t abb_tamanio(abb_t *arbol)
{
	return !arbol ? 0 : arbol->tamanio;
}

void abb_destruir(abb_t *arbol)
{
	abb_destruir_todo(arbol, NULL);
}

void abb_destruir_todo(abb_t *arbol, void (*destructor)(void *))
{
	if (!arbol)
		return;

	nodo_destruir(arbol->nodo_raiz, destructor);
	free(arbol);
}

size_t abb_con_cada_elemento(abb_t *arbol, abb_recorrido recorrido,
			     bool (*funcion)(void *, void *), void *aux)
{
	if (!arbol || !funcion || abb_vacio(arbol))
		return 0;

	size_t contador = 0;

	void *auxiliar = !aux ? (void *)&contador : aux;

	switch (recorrido) {
	case PREORDEN:
		nodo_con_cada_elemento_preorden(arbol->nodo_raiz, funcion,
						auxiliar, &contador);
		break;
	case INORDEN:
		nodo_con_cada_elemento_inorden(arbol->nodo_raiz, funcion,
					       auxiliar, &contador);
		break;
	case POSTORDEN:
		nodo_con_cada_elemento_postorden(arbol->nodo_raiz, funcion,
						 auxiliar, &contador);
		break;
	}
	return contador;
}

size_t abb_recorrer(abb_t *arbol, abb_recorrido recorrido, void **array,
		    size_t tamanio_array)
{
	if (!arbol || !array || abb_vacio(arbol))
		return 0;

	recorrer_array_t array_recorrido = { array, tamanio_array, 0 };

	return abb_con_cada_elemento(arbol, recorrido, copiar_array,
				     &array_recorrido);
}