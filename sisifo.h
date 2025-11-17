#ifndef SISIFO_H
#define SISIFO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_ARTICULOS 200
#define RUTA "archivos.txt"

typedef struct {
    char autorNombre[100];
    char autorApellido[100];
    char titulo[300];
    char ruta[300];
    int  anio;
    char resumen[500];
} Articulo;

// Comparador genérico para strings 
typedef int (*ComparadorTexto)(const Articulo*, const Articulo*);

// ---------------- PROTOTIPOS ----------------

// Carga y muestra
int cargarIndice(const char *nombreArchivo, Articulo lista[], int maxArticulos);
void mostrarArticulos(Articulo lista[], int n);

// utilidades
void split(char *linea, char *campos[], int *numCampos, const char *sep);
void intercambiar(Articulo *a, Articulo *b);
int contarPalabras(const char *texto);

// comparadores
int compararPorTitulo(const Articulo *a, const Articulo *b);
int compararPorRuta(const Articulo *a, const Articulo *b);
int obtenerClaveNumerica(const Articulo *a);

// heaps textuales genéricos
void heapifyTexto(Articulo arr[], int n, int i, ComparadorTexto cmp);
void construirHeapTexto(Articulo arr[], int n, ComparadorTexto cmp);

// heaps numéricos
void heapifyNumerico(Articulo arr[], int n, int i);
void construirHeapNumerico(Articulo arr[], int n);

// ordenamientos
void heapSortPorTitulo(Articulo arr[], int n);
void heapSortPorTamTitulo(Articulo arr[], int n);
void heapSortPorRuta(Articulo arr[], int n);

#endif
