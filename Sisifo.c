#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "sisifo.h"

void split(char *linea, char *campos[], int *numCampos, const char *sep) {
    char *token = strtok(linea, sep);
    int i = 0;

    while (token != NULL) {
        campos[i++] = token;
        token = strtok(NULL, sep);
    }

    *numCampos = i;
}

int cargarIndice(const char *nombreArchivo, Articulo lista[], int maxArticulos) {
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

void mostrarArticulos(Articulo lista[], int n) {
    for (int i = 0; i < n; i++) {
        printf("\n---- Artículo %d ----\n", i+1);
        printf("Título: %s\n", lista[i].titulo);
        printf("Autor: %s %s\n", lista[i].autorNombre, lista[i].autorApellido);
        printf("Año: %d\n", lista[i].anio);
        printf("Ruta: %s\n", lista[i].ruta);
        printf("Resumen: %s\n", lista[i].resumen);
    }
}
//-------------------------HEAP TEXTUAL-------------------------
void heapifyTexto(Articulo arr[], int n, int i, ComparadorTexto cmp) {
    int mayor = i;          
    int izq = 2 * i + 1;    
    int der = 2 * i + 2;    

    if (izq < n && cmp(&arr[izq], &arr[mayor]) > 0) {
        mayor = izq;
    }

    if (der < n && cmp(&arr[der], &arr[mayor]) > 0) {
        mayor = der;
    }

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
    // desde el ultimo nodo interno hacia la raíz
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapifyNumerico(arr, n, i);
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









int main() {
    Articulo lista[MAX_ARTICULOS];
    int cantidad = cargarIndice(RUTA, lista, MAX_ARTICULOS);

    int opcion;
    int mostrar;

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
        printf("4.  Ordenar por (custom falta)\n");
        printf("5. SALIR\n");
        printf("===========================================\n");
        printf("Seleccione una opción: ");

        scanf("%d", &opcion);

        switch(opcion) {
            case 1:
                heapSortPorTitulo(lista, cantidad);
                printf("\nArtículos ordenados por TÍTULO:\n");
                break;

            case 2:
                heapSortPorTamTitulo(lista, cantidad);
                printf("\nArtículos ordenados por CANTIDAD DE PALABRAS del título:\n");
                break;

            case 3:
                heapSortPorRuta(lista, cantidad);
                printf("\nArtículos ordenados por NOMBRE DE ARCHIVO:\n");
                break;

            case 4:
                //TODO hacer el ultimo ordenamiento
                break;

            case 5:
                printf("Saliendo...\n");
                break;

            default:
                printf("\nOpción no válida. Intente de nuevo.\n");
                continue;
        }

        if (opcion >= 1 && opcion <= 4) {
            printf("\n¿Cuántos artículos desea mostrar? (1 - %d): ", cantidad);
            scanf("%d", &mostrar);

            if (mostrar < 1) mostrar = 1;
            if (mostrar > cantidad) mostrar = cantidad;

            printf("\n=========== RESULTADOS ===========\n");
            for (int i = 0; i < mostrar; i++) {
                printf("\n--- Artículo %d ---\n", i+1);
                printf("Título: %s\n", lista[i].titulo);
                printf("Resumen: %s\n", lista[i].resumen);
                printf("Ruta: %s\n", lista[i].ruta);
            }
            printf("\n==================================\n");
        }

    } while(opcion != 5);

    return 0;
}

