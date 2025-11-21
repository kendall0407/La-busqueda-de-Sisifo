#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "sisifo.h"

void split(char *linea, char *campos[], int *numCampos, const char *sep) {
	/**
	 * Separa una línea en campos usando el separador dado con ayuda de strtok
     * Dividiendo cada fragmento en tokens para almacenarlos en el arreglo campos
     * const char para solo usar esa cadena como separador, no modificarla
     * Modifica numCampos y llena campos con los tokens encontrados
	 */

    char *token = strtok(linea, sep);
    int i = 0;

    while (token != NULL) {
        campos[i++] = token;
        token = strtok(NULL, sep);
    }

    *numCampos = i;
}

int cargarIndice(const char *nombreArchivo, Articulo lista[], int maxArticulos) {
    /**
     * Carga los artículos desde un archivo de texto
     * Cada linea del archivo representa un artículo con campos separados por |
     * Llena el arreglo lista con los articulos cargados
     * Retorna la cantidad de artículos cargados
    */

    FILE *f = fopen(nombreArchivo, "r");
    if (!f) {
        printf("Error: no se pudo abrir el archivo %s\n", nombreArchivo);
        return 0;
    }

    char linea[2000];
    int cantidad = 0;

    while (fgets(linea, sizeof(linea), f) != NULL && cantidad < maxArticulos) {
        // elimina el enter final
        linea[strcspn(linea, "\n")] = 0;

        // lista tokens
        char *campos[10];
        int numCampos = 0;

        split(linea, campos, &numCampos, "|");

        if (numCampos < 6)
            continue;   //si esta incompleta pasa
        // llenar struct
        strcpy(lista[cantidad].autorNombre, campos[0]);
        strcpy(lista[cantidad].autorApellido, campos[1]);
        strcpy(lista[cantidad].titulo, campos[2]);
        strcpy(lista[cantidad].ruta, campos[3]);
        lista[cantidad].anio = atoi(campos[4]);
        strcpy(lista[cantidad].resumen, campos[5]);

        cantidad++;
    }

    fclose(f);
    return cantidad;
}

void mostrarArticulos(Articulo lista[], int n, int *listaApariciones) {
    // Recorrer y printear los artiuclos

    printf("\n=========== RESULTADOS ===========\n");
    for (int i = 0; i < n; i++) {
        printf("\n--- Artículo %d ---\n", i+1);
        printf("Título: %s\n", lista[i].titulo);
        printf("Resumen: %s\n", lista[i].resumen);
        printf("Ruta: %s\n", lista[i].ruta);

        if (listaApariciones != NULL) {
            printf("Apariciones: %d\n", listaApariciones[i]);
        }
    }
    printf("\n==================================\n");
}

//-------------------------HEAP TEXTUAL-------------------------
void heapifyTexto(Articulo arr[], int n, int i, ComparadorTexto cmp) {
    /**
     * Funcion heap maximo para comparadores de texto
     * compara un nodo con sus dos hijos
     * Elige el mas grande (segun el comparador de texto que se pase)
     * Si el padre no era el mas grande
     * lo intercambia con el hijo mas grande y repite el proceso recursivamente
    */

    int mayor = i;          // indice del mayor
    int izq = 2 * i + 1;    // hijo izquierdo
    int der = 2 * i + 2;    // hijo derecho

    // si hijo izquierdo existe y su año es mayor
    if (izq < n && cmp(&arr[izq], &arr[mayor]) > 0) {
        mayor = izq;
    }

    // si hijo derecho existe y su año es mayor
    if (der < n && cmp(&arr[der], &arr[mayor]) > 0) {
        mayor = der;
    }

    // si el mayor no es la raiz
    if (mayor != i) {
        intercambiar(&arr[i], &arr[mayor]);
        heapifyTexto(arr, n, mayor, cmp);
    }
}

void construirHeapTexto(Articulo arr[], int n, ComparadorTexto cmp) {
    // se commienza desde el ultimo nodo
    for (int i = n/2 - 1; i >= 0; i--) {
        heapifyTexto(arr, n, i, cmp);
    }
}

//-------------------------HEAP NUMERICO-------------------------
    /**
     * Funcion heap maximo numerico
     * compara un nodo con sus dos hijos
     * Elige el mas grande (segun el comparador de texto que se pase)
     * Si el padre no era el mas grande
     * lo intercambia con el hijo mas grande y repite el proceso recursivamente
     * misma idea que el textual pero con numeros
    */

void heapifyNumerico(Articulo arr[], int n, int i) {
    int mayor = i;          // indice del mayor
    int izq = 2 * i + 1;    // hijo izquierdo
    int der = 2 * i + 2;    // hijo derecho

    // si hijo izquierdo existe y su año es mayor
    if (izq < n && obtenerClaveNumerica(&arr[izq]) > obtenerClaveNumerica(&arr[mayor])) {
        mayor = izq;
    }

    // si hijo derecho existe y su año es mayor
    if (der < n && obtenerClaveNumerica(&arr[der]) > obtenerClaveNumerica(&arr[mayor])) {
        mayor = der;
    }

    // si el mayor no es la raiz
    if (mayor != i) {
        intercambiar(&arr[i], &arr[mayor]);
        heapifyNumerico(arr, n, mayor);
    }
}



void construirHeapNumerico(Articulo arr[], int n) {
    // Desde el ultimo nodo interno hacia la raíz
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapifyNumerico(arr, n, i);
    }
}



void heapifyNumericoApariciones(int apariciones[], Articulo lista[], int n, int i) {
    /** 
    * la diferencia de este con la funcion anterior es que esta ordena un arreglo de enteros
    * que representan las apariciones de una palabra en los articulos, era necesario otra mas
    * para mantener la alineacion entre ambos arreglos y no hacerlo mas complicado
    */
    int mayor = i;          // indice del mayor
    int izq = 2 * i + 1;    // hijo izquierdo
    int der = 2 * i + 2;    // hijo derecho

    if (izq < n && apariciones[izq] > apariciones[mayor]) {
        mayor = izq;
    }
    
    if (der < n && apariciones[der] > apariciones[mayor]) {
        mayor = der;
    }

    // si el mayor no es la raiz
    if (mayor != i) {
        // intercambia apariciones para mantener alineacion con los articulos
        int temp = apariciones[i];
        apariciones[i] = apariciones[mayor];
        apariciones[mayor] = temp;

        // intercambiar articulos         
        intercambiar(&lista[i], &lista[mayor]);
        heapifyNumericoApariciones(apariciones, lista, n, mayor);
    }
}

void construirHeapApariciones(int apariciones[], Articulo lista[], int n) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapifyNumericoApariciones(apariciones, lista, n, i);
    }
}



//-------------------------ORDENAMIENTO 1(TITULO)-------------------------
int compararPorTitulo(const Articulo *a, const Articulo *b) {
    return strcmp(a->titulo, b->titulo);
}
void intercambiar(Articulo *a, Articulo *b) {
    Articulo temp = *a;
    *a = *b;
    *b = temp;
}


void heapSortPorTitulo(Articulo arr[], int n) {
    // construir comnticulo maximo
    construirHeapTexto(arr, n,compararPorTitulo);

    // extraer elementos 1 por 1
    for (int i = n - 1; i > 0; i--) {
        // mover el maximo al final
        intercambiar(&arr[0], &arr[i]);

        // llamar heapify en el heap reducido
        heapifyTexto(arr, i, 0,compararPorTitulo);
    }
}
//-------------------------ORDENAMIENTO 2(Tamanio del titulo)-------------------------
int contarPalabras(const char *texto) {
    int enPalabra = 0;
    int conteo = 0;

    for (int i = 0; texto[i] != '\0'; i++) {
        if (!isspace((unsigned char)texto[i]) && enPalabra == 0) {
            enPalabra = 1;
            conteo++;
        } 
        else if (isspace((unsigned char)texto[i])) {
            enPalabra = 0;
        }
    }

    return conteo;
}
int obtenerClaveNumerica(const Articulo *a) {
    return contarPalabras(a->titulo);
}
void heapSortPorTamTitulo(Articulo arr[], int n) {
    // construir heap basado en cantidad de palabras
    construirHeapNumerico(arr, n);

    for (int i = n - 1; i > 0; i--) {
        intercambiar(&arr[0], &arr[i]);
        heapifyNumerico(arr, i, 0);
    }
}
//-------------------------ORDENAMIENTO 3(Nombre del archivo)-------------------------
int compararPorRuta(const Articulo *a, const Articulo *b) {
    return strcmp(a->ruta, b->ruta);
}
void heapSortPorRuta(Articulo arr[], int n) {
    construirHeapTexto(arr, n, compararPorRuta);

    for (int i = n - 1; i > 0; i--) {
        intercambiar(&arr[0], &arr[i]);
        heapifyTexto(arr, i, 0, compararPorRuta);
    }
}

//------------------------ORDENAMIENTO 4(Apariciones de palabra)------------------------

int contarApariciones(const char *texto, const char *palabra) {
    /**
     * Cuenta cuantas veces aparece una palabra en un texto
     * busca ocurrencias de la palabra en el texto y las cuenta
     * Retorna el num de apariciones encontradas
    */
    int contador = 0;
    const char *p = texto;  // un puntero p que empieza al inicio del texto.
    int len = strlen(palabra);

    while ((p = strstr(p, palabra)) != NULL) {
        contador++;
        p += len; // avanzar el puntero
    }

    return contador;
}

int* consultarPalabra(Articulo lista[], int cantidad) {
    /**
     * Solicita al usuario una palabra
     * cuenta cuantas veces aparece en cada artiuclo
     * ordena los arrticulos según la cantidad de apariciones (de mayor a menor)
     * retorna un arreglo con la cantidad de apariciones por articuo, unicamente
     * para imprimir luego en mostrarArticulos
    */

    // Limpiar el buffer de entrada
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
	char* palabra = calloc(50, sizeof(char)); // Se reservan 50 bytes como maximo
	printf("\nDigite la palabra que desea buscar: ");
	fgets(palabra, 50, stdin);		// fgets(para variar con scanf), maximo 50 bytes acepta 
	palabra[strcspn(palabra, "\n")] = '\0';
	
	// Buscar la palabra en cada archivo y ordenarlo segun cantidad de apariciones
    int *listaApariciones = calloc(cantidad, sizeof(int));
    for (int i = 0; i < cantidad; i++) {
        listaApariciones[i] = contarApariciones(lista[i].titulo, palabra);
    }
    // Ordenar usando heap sort basado en listaApariciones
    construirHeapApariciones(listaApariciones, lista, cantidad);
    
    for (int i = cantidad - 1; i > 0; i--) {
        // intercambiar apariciones
        int tmp = listaApariciones[0];
        listaApariciones[0] = listaApariciones[i];
        listaApariciones[i] = tmp;

        // intercambiar artículos para mantener alineacion
        intercambiar(&lista[0], &lista[i]);
        heapifyNumericoApariciones(listaApariciones, lista, i, 0);
    }
    printf("\nArtículos ordenados por cantidad de apariciones de la palabra '%s':\n", palabra);
    return listaApariciones;
    free(listaApariciones);
    free(palabra);
}






int main() {
    // Se definen los articulos y se carga el indice de articulos cargados desde el archivo
    Articulo lista[MAX_ARTICULOS];
    int cantidad = cargarIndice(RUTA, lista, MAX_ARTICULOS);

    int opcion;
    int mostrar;
    int *listaApariciones = NULL;   
    if (cantidad == 0) {
        printf("No se cargaron artículos.\n");
        return 0;
    }

    printf("Cargados: %d artículos\n", cantidad);

    do {
        printf("\n========== MENU DE ORDENAMIENTOS ==========\n");
        printf("1. Ordenar por TÍTULO\n");
        printf("2. Ordenar por TAMAÑO DEL TÍTULO\n");
        printf("3. Ordenar por NOMBRE DE ARCHIVO\n");
        printf("4. Ordenar por PALABRA\n");
        printf("5. SALIR\n");
        printf("===========================================\n");
        printf("Seleccione una opción: ");

        scanf("%d", &opcion);

        switch(opcion) {
            case 1:
                // Ordenar por título
                heapSortPorTitulo(lista, cantidad);
                printf("\nArtículos ordenados por TÍTULO:\n");
                break;

            case 2:
                // Ordenar por tamaño del título
                heapSortPorTamTitulo(lista, cantidad);
                printf("\nArtículos ordenados por CANTIDAD DE PALABRAS del título:\n");
                break;

            case 3:
                // Ordenar por nombre de archivo
                heapSortPorRuta(lista, cantidad);
                printf("\nArtículos ordenados por NOMBRE DE ARCHIVO:\n");
                break;

            case 4:
                // Ordenar por palabra
				listaApariciones = consultarPalabra(lista, cantidad);
                break;

            case 5:
                printf("Saliendo...\n");
                break;

            default:
                printf("\nOpción no válida. Intente de nuevo.\n");
                continue;
        }
        // Mostrar artículos si la opción es válida
        if (opcion >= 1 && opcion <= 4) { 
            printf("\n¿Cuántos artículos desea mostrar? (1 - %d): ", cantidad);
            while (scanf("%d", &mostrar) != 1) {
				printf("\nError, escribe un valor válido por favor (1-5)\n");
				scanf("%*s");
			}
			if (mostrar >= 1 && mostrar <= cantidad) {            
				mostrarArticulos(lista, mostrar, listaApariciones);
			}
        }

    } while(opcion != 5);

    return 0;
}

