<div align="right">
<img width="40px" src="img/algo2.svg">
</div>

# TP: Carrera de obstáculos

## Repositorio de Felipe Perassi - 107630 - fperassi@fi.uba.ar

## Introducción

En el presente trabajo práctico, se desarrolló el TP integrador de la materia, el cual consiste en implementar una carrera de obstáculos pokemon, realizando la interacción con el usuario a través de la terminal.

El juego consiste en las siguientes reglas que se explican a continuación:

- Al iniciar el juego, el jugador selecciona la dificultad que desea (facil, normal, difícil, imposible).
- Luego de seleccionada la dificultad se selecciona al azar un pokemon y se crea una pista de obstáculos.
- Se le presenta al jugador la información del pokemon contra el cuál va a competir y se le muestra información acerca de la longitud y los obstáculos de la pista
  - Se informa la cantidad y posición de los obstáculos pero se omite información acerca de alguno de los obstáculos.
  - La cantidad de obstáculos omitidos depende de la dificultad.
- Se le pide al jugador que seleccione su propio pokemon y que arme su propia pista de obstáculos (no hay restricción de tamaño).
- Cuando el jugador termina de armar la pista, se corre una carrera entre ambos pokemones.
- El objetivo del juego es que ambos pokemon terminen la carrera de obstáculos al mismo tiempo o lo mas cercano posible.
- Una vez finalizada la carrera, el jugador puede modificar la pista (no asi cambiar su pokemon) y volver a correr para mejorar su puntaje.
  - La cantidad de veces que el jugador puede reintentar la carrera depende de la dificultad.

Para llevar a cabo esto, se dispone de un TDA TP y dos archivos para generar el menu, el 'menu.c' que se encarga de imprimir por pantalla los diseños del menu y el 'menu_interactivo.c' que maneja la interacción del usuario con el menu. 

La estructura del TDA TP está compuesta por un ABB, donde se guardan los pokemones leídos del archivo .txt; un vector de 2 elementos de tipo const struct pokemon_info, para asignar el pokemon que selecciona cada uno de los jugadores que juegan y un vector de 2 listas, para almacenar los obstáculos de cada jugador. Se implementaron las funciones básicas para manejar el TDA, como crear y destruir el TP. Además, se implementaron funciones adicionales que proporcionan todo tipo de información sobre el tp, los pokemones y los obstáculos de los jugadores. También se implementaron funciones que devuelven strings dinámicas con información necesaria, que luego el usuario se debe encargar de liberar.

Por otro lado, el menu y el menu_interactivo tienen funciones propias para poder imprimir e interactuar por terminal con el usuario, respectivamente. Además, su estructura incluye el TP y la dificultad del juego para poder encapsularlos y que el 'juego.c' no dependa de los mismos.

---

## Compilación

- Para compilar el juego:
<div align="center">

```bash
make compilar_juego
```
</div>

- Para compilar las pruebas del alumno:

<div align="center">

```bash
make compilar_pruebas_alumno
```
</div>

## Ejecución

- Para ejecutar el juego pasando un archivo con los pokemones por defecto:

<div align="center">

```bash
make juego_default
```
</div>

- Para ejecutar las pruebas del alumno:

<div align="center">

```bash
make pruebas_alumno
```
</div>

## Valgrind

- Para ejecutar el juego pasando un archivo con los pokemones por defecto con valgrind:
<div align="center">

```bash
make valgrind_juego_default
```
</div>

- Para ejecutar las pruebas del alumno con valgrind:
<div align="center">

```bash
make valgrind_alumno
```
</div>

---

## Información importante

Para cargar un archivo y utilizarlo en el juego, se lo debe poner en la carpeta llamada "archivos". Algo que se menciona en el juego es que se pueden poner hasta 25 pokemones en el archivo, esto no se implementó a nivel código, sino que se le pide al usuario que no se carguen más de esa cantidad para que la interfaz mantenga su estructura. Se destaca que el archivo debe ser un '.txt' y tener el formato especificado a continuación para cada uno de los pokemones.

<div align="center">

nombre1,fuerza1,destreza1,inteligencia1 

nombre2,fuerza2,destreza2,inteligencia2

... 

nombreN,fuerzaN,destrezaN,inteligenciaN 

</div>

Luego, una vez dispuesto el archivo '.txt' con el formato especificado en la carpeta mencionada, se debe compilar el juego con las lineas mostradas anteriormente y se lo debe ejecutar con las siguientes lineas:

- Para ejecutar el juego:

<div align="center">

```bash
./juego "archivos/<nombre_de_tu_archivo>.txt"
```
</div>

- Para ejecutar el juego con valgrind:

<div align="center">

```bash
valgrind $(VALGRIND_FLAGS) ./juego "archivos/<nombre_de_tu_archivo>.txt"
```
</div>

Siendo los valgrind flags que se usaron los que se ven a continuación.

<div align="center">

```bash
VALGRIND_FLAGS=--leak-check=full --track-origins=yes --show-reachable=yes --error-exitcode=2 --show-leak-kinds=all --trace-children=yes
```
</div>

---

##  Funcionamiento

### TDA TP

En esta sección, nos adentraremos en la implementación del TDA correspondiente al funcionamiento del TP. Exploraremos en detalle cómo se han desarrollado tanto las funciones solicitadas por la cátedra como las extras, para mejorar la legibilidad del código. 

En primer lugar, se pensó la estructura a utilizar, que fue `TP`. Además de la ya proporcionada por la cátedra, que es `pokemon_info`, a la cual se la renombró como `pokemon_t` en el archivo tp.c.

Esta última posee los parámetros básicos del pokemon como el nombre, la fuerza, la destreza y la inteligencia del mismo, características que se leen pura y exclusivamente del archivo dado con el formato especificado.

Por otro lado, la primera estructura se utiliza para almacenar toda la información relevante para el funcionamiento del tp y posee los siguientes campos:

- `abb_t *pokemones`: Un árbol binario de búsqueda que almacena los pokemones leídos en el archivo '.txt'. 
- `pokemon_t *pokemon_jugador[2]`: Un array de dos punteros que almacena los pokemones seleccionados por los dos jugadores.
- `lista_t *obstaculos_pista[2]`: Dos listas que representan los obstáculos en la pista para cada jugador.

Más adelante se explica en detalle cada una de las funciones más importantes del TDA, pero se debe comentar que para los pokemones se eligió un ABB, principalmente debido a su ordenamiento y la fácil accesibilidad a mantener los elementos ordenados ascendentemente, realizando un recorrido inorder. Además de sus ya estudiadas complejidades en cuanto a la inserción, eliminación y búsqueda.

También, cabe destacar que se eligió una lista para los obstáculos, debido a que estas mantienen el orden de inserción de los elementos que se le agregan. De la misma forma que para el ABB, se tuvo en cuenta la conocida eficiencia en cuanto a las funciones de este TDA que se utilizarán en el TP. 

A partir de esto, se desarrollaron las diferentes funciones solicitadas por la cátedra y algunas extra para mejorar la legibilidad del código realizado. A continuación se realiza una breve descripción de cada una de las funciones más importantes y su complejidad computacional.

- `tp_crear`: se encarga de inicializar la estructura tp, leer la información de los pokemones desde un archivo y crear las listas de obstáculos. La complejidad de esta función depende principalmente de la lectura y almacenamiento de los pokemones en el ABB. La lectura de cada línea del archivo tiene una complejidad O(1) y si hay n pokemones, insertarlos en el ABB, que es balanceado, tiene una complejidad de O(nlogn). La inicialización de las listas de obstáculos es de complejidad O(1) para cada una, resultando en un total de O(nlogn) para la función completa.

- `tp_cantidad`: Esta función simplemente retorna la cantidad de pokemones almacenados en el ABB. Dado que esta información se puede mantener actualizada durante las operaciones de inserción y eliminación en el ABB, la complejidad de esta función es O(1), ya que, acceder a un contador almacenado es una operación constante.

- `tp_buscar_pokemon`: Busca un pokemon por su nombre en el ABB. La búsqueda en un ABB balanceado tiene una complejidad de O(logn), donde n es el número de pokemones en el ABB. Esta complejidad es logarítmica porque, en el peor de los casos, cada comparación descarta la mitad de los elementos restantes.

- `tp_nombres_disponibles`: Se realiza un recorrido en orden del ABB para concatenar los nombres de todos los pokemones en una cadena. Un recorrido en orden de un ABB tiene una complejidad de O(n), donde n es el número de nodos, ya que se visitan todos los nodos exactamente una vez. La concatenación de los nombres en una cadena puede añadir una sobrecarga adicional, pero asumimos que esto es manejado eficientemente, por lo tanto, la complejidad de la función es O(n).

- `tp_seleccionar_pokemon`: Selecciona un pokemon para un jugador, primero buscando el pokemon en el ABB y luego asignándolo a un jugador. La búsqueda en el ABB tiene una complejidad de O(log n), y la asignación a un jugador es una operación constante, O(1). Por ende, la complejidad total es O(log n).

- `tp_pokemon_seleccionado`: Esta función simplemente retorna el pokemon seleccionado por un jugador. Acceder a un elemento en un array tiene una complejidad de O(1). De esta manera, esta función también tiene una complejidad constante, O(1).

- `tp_agregar_obstaculo`: Se agrega un obstáculo a la pista de un jugador utilizando una lista. La inserción en una posición dada de la lista implementada con nodos simplemente enlazados tiene una complejidad en el peor de los casos O(n), ya que, si la posición es el final se debe recorrer toda la lista. 

- `tp_quitar_obstaculo`: Para una lista simplemente enlazada, quitar un obstáculo en una posición específica implica recorrer la lista hasta el nodo anterior al nodo que se quiere eliminar. Esto tiene una complejidad de O(n), en el peor de los casos, ya que, cuando se elimina un nodo al final de la lista, la complejidad sería O(n), donde n es el número de elementos en la lista. De igual forma, una vez que se llega a la posición, la eliminación en sí misma es O(1).

- `tp_obstaculos_pista`: Esta función recorre toda la lista de obstáculos y concatena los nombres de los obstáculos en una cadena. En una lista simplemente enlazada, recorrer toda la lista tiene una complejidad de O(n), donde n es el número de obstáculos. La concatenación de nombres puede tener una complejidad adicional, pero en general, la operación principal es el recorrido, por lo que la complejidad total de la función es O(n).

- `tp_limpiar_pista`: se eliminan todos los obstáculos de la pista de un jugador. Si hay n obstáculos en la lista, eliminar cada uno implica simplemente actualizar el puntero de la cabeza de la lista para que apunte al siguiente nodo, lo que es O(1) por eliminación. Por lo tanto, la complejidad total es O(n).

- `tp_calcular_tiempo_pista`: Se recorre la lista de obstáculos y se calcula el tiempo total basado en el atributo correspondiente del pokemon del jugador. Recorrer la lista y realizar una operación constante por cada obstáculo tiene una complejidad de O(n), donde n es el número de obstáculos.

- `tp_tiempo_por_obstaculo`: Esta función recorre la lista de obstáculos y construye una cadena en formato csv con el tiempo necesario para cada obstáculo. Similar a tp_obstaculos_pista, recorrer la lista y construir la cadena tiene una complejidad de O(n), donde n es el número de obstáculos.

- `tp_destruir`: Libera toda la memoria utilizada por la estructura tp. Destruir un ABB tiene una complejidad de O(n), donde 
n es el número de nodos en el ABB. Recorrer y destruir cada lista de obstáculos tiene una complejidad de O(n) por lista. Por lo tanto, la complejidad total de esta función es O(n) + O(n), que es lo mismo que O(n).

### TDA MENU

El TDA menu se dividió en dos archivos: uno se encarga del printeo general en la términal (menu), mientras que el otro se encarga de la interacción con el usuario (menu_interaccion). Esto se decidió así, dado que, de esta manera se espera mejorar la legibilidad del código.

Este TDA consta de la siguiente estructura:

- `TP *tp`: Un puntero a un TP para que el menu encapsule la lógica empleada en el TDA TP del main de 'juego.c'. 
- `enum MENU_DIFICULTAD dificultad`: Representa los diferentes niveles de dificultad disponibles para el juego.

Una vez entendida esta estructura, se describe la implementación del 'menu.c' y la del 'menu_interactivo.c'. 

En primer lugar, el código desarollado en el archivo 'menu.c' organiza la interacción con el usuario a través de diversas pantallas y menús, utilizando funciones auxiliares para imprimir texto y gráficos en la terminal, que luego serán utilizadas por la interacción del usuario. También, en cuanto a la lógica necesaria del juego, se manejan pokémones y pistas con funciones específicas para imprimir información, seleccionar pokémones automáticamente de manera aleatoria (para el jugador 2), insertar obstáculos y calcular puntajes basados en el tiempo de carrera.

Por otro lado, a partir de lo codeado en el archivo 'menu_interactivo.c', se obtiene un sistema que permite al usuario interactuar con diferentes aspectos del juego a través de la consola, desde la selección de opciones principales hasta la gestión detallada de acciones dentro del juego, como la selección de pokemones, la gestión de obstáculos y la ejecución de la carrera. Cada una de las funciones en este archivo están diseñadas para proporcionarle al usuario una experiencia comprensible a través de mensajes claros y funciones de validación de entrada. Estas funciones luego son utilizadas en el 'juego.c', dentro de ellas, las más importantes son:

- `menu_crear`: Se encarga de inicializar y configurar un nuevo menú de juego, asignando memoria para la estructura menu_t y cargando los datos iniciales desde un archivo especificado por nombre_archivo. Retorna un puntero a la estructura menu_t creada, que contiene los datos y configuraciones necesarios para el juego.

- `menu_interactivo_pantalla_inicial`: Esta función muestra la pantalla inicial del juego, esperando a que el usuario confirme para continuar. Después de la confirmación, limpia la pantalla para prepararla para la siguiente interacción.

- `menu_interactivo_pantalla_principal`: Se encarga de desplegar el menú principal del juego y permite al usuario elegir entre dos opciones numeradas. Retorna el número seleccionado por el usuario (1 o 2), indicando la dirección que desea tomar dentro del juego.

- `menu_interactivo_pantalla_cambiar_archivo`: En este menu, se le presenta al usuario la opción de cambiar el archivo de configuración del juego y se le indican las instrucciones para que lo haga. Espera a que el usuario confirme la acción antes de limpiar la pantalla para finalizar el juego y que el usuario pueda cambiar el archivo.

- `menu_interactivo_pantalla_seleccion_dificultad`: Se le muestra al usuario las opciones de dificultad disponibles (fácil, medio, difícil, imposible) y permite seleccionar una. La dificultad elegida se asigna a la estructura menu_t proporcionada como argumento.

- `menu_interactivo_pantalla_seleccion_pokemon_maquina`: Esta función muestra al usuario la pantalla para la selección del pokémon por parte de la máquina. Espera a que el usuario aprete enter antes de limpiar la pantalla para continuar.

- `menu_interactivo_pantalla_pista_pokemon_maquina`: Se encarga de mostrar al usuario información relevante sobre la pista del pokémon seleccionado por la máquina, como las posiciones de los obstáculos o su tipo. Espera a que el usuario confirme antes de limpiar la pantalla para continuar con la ejecución del juego.

- `menu_interactivo_pantalla_seleccion_pokemon_usuario`: Permite al usuario seleccionar su pokémon para jugar. Guía al usuario a través del proceso de selección interactiva, validando las elecciones y mostrando feedback en la pantalla antes de limpiarla para continuar.

- `menu_interactivo_pantalla_juego`: Esta función muestra la pantalla principal del juego, donde el usuario puede interactuar con diversas opciones como agregar obstáculos, participar en carreras, obtener información del oponente o propia. Espera a que el usuario seleccione una opción antes de limpiar la pantalla para continuar con los submenues correspondientes a cada acción del juego.

- `menu_destruir`: Libera toda la memoria asignada dinámicamente para el menú creado, incluyendo lo utilizado por el TP. Es importante llamar a esta función al finalizar el juego para evitar pérdidas de memoria y asegurar una correcta limpieza de recursos utilizados.

## Conclusión

El presente trabajo práctico ha proporcionado una experiencia integral en el desarrollo y la implementación de un juego de carreras de pokémones ejecutado en la terminal. A través del uso de C para la codificación y Markdown para la redacción del informe, se logró alcanzar el objetivo esperado, explorando diversos conceptos abordados a lo largo del curso. Este proyecto no solo involucró la creación del código necesario, sino también la conceptualización y el diseño de una interfaz gráfica adaptada al entorno de la terminal.

En base a lo realizado en este TP, se ha obtenido, un código modularizado, que pretende cumplir con las buenas prácticas de la programación. También, se ha cumplido con la metodología orientada a pruebas que la cátedra recomendaba, habiendo realizado las pruebas que se consideraron pertinentes en el archivo "pruebas_alumno.c".

El diseño del juego no solo se enfocó en la eficiencia lógica, sino también en la claridad y comprensión de la interfaz gráfica para el usuario final. Se implementaron mecanismos que facilitan la interacción intuitiva y la navegación dentro del juego.

Por último, se realizó un informe detallado que abarca la explicación del diseño del juego, la descripción del código implementado y un análisis de la complejidad de las principales funciones desarrolladas.
