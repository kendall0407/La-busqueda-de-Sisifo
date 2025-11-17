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



int main() {
    Articulo lista[MAX_ARTICULOS];

    int cantidad = cargarIndice("archivos.txt", lista, MAX_ARTICULOS);

    printf("Artículos cargados: %d\n", cantidad);

    for (int i = 0; i < cantidad; i++) {
        printf("---- Artículo %d ----\n", i + 1);
        printf("Autor: %s %s\n", lista[i].autorNombre, lista[i].autorApellido);
        printf("Título: %s\n", lista[i].titulo);
        printf("Ruta: %s\n", lista[i].ruta);
        printf("Año: %d\n", lista[i].anio);
        printf("Resumen: %s\n\n", lista[i].resumen);
    }

    return 0;
}
