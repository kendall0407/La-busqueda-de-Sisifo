#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ARTICULOS 200

typedef struct {
    char autorNombre[100];
    char autorApellido[100];
    char titulo[300];
    char ruta[300];
    int  anio;
    char resumen[500];
} Articulo;


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



//-------------------------ORDENAMIENTO 1(TITULO)-------------------------
int compararPorTitulo(const Articulo *a, const Articulo *b) {
    return strcmp(a->titulo, b->titulo);
}
void intercambiar(Articulo *a, Articulo *b) {
    Articulo temp = *a;
    *a = *b;
    *b = temp;
}
void heapifyTexto(Articulo arr[], int n, int i) {
    int mayor = i;          
    int izq = 2 * i + 1;    
    int der = 2 * i + 2;    

    // Comparar hijo izquierdo
    if (izq < n && compararPorTitulo(&arr[izq], &arr[mayor]) > 0) {
        mayor = izq;
    }

    // Comparar hijo derecho
    if (der < n && compararPorTitulo(&arr[der], &arr[mayor]) > 0) {
        mayor = der;
    }

    // Si encontramos un hijo mayor, intercambiamos y recursamos
    if (mayor != i) {
        intercambiar(&arr[i], &arr[mayor]);
        heapifyTexto(arr, n, mayor);
    }
}
void construirHeapTexto(Articulo arr[], int n) {
    // Comenzamos desde el último nodo interno
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapifyTexto(arr, n, i);
    }
}

void heapSortPorTitulo(Articulo arr[], int n) {
    // 1. Construir montículo máximo
    construirHeapTexto(arr, n);

    // 2. Extraer elementos uno por uno
    for (int i = n - 1; i > 0; i--) {
        // Mover la raíz (máximo) al final
        intercambiar(&arr[0], &arr[i]);

        // Llamar heapify en el heap reducido
        heapifyTexto(arr, i, 0);
    }
}





int main() {
    Articulo lista[MAX_ARTICULOS];
    int cantidad = cargarIndice("archivos.txt", lista, MAX_ARTICULOS);

    printf("Cargados: %d artículos\n", cantidad);

    heapSortPorTitulo(lista, cantidad);

    printf("\nArtículos ordenados por título:\n");
    mostrarArticulos(lista, cantidad);

    return 0;
}
