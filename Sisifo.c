#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "sisifo.h"

/**
 *  Divide una línea en tokens usando un separador.
 *
 * Esta función utiliza `strtok` para dividir la cadena `linea` en
 * tokens separados por la cadena `sep`. Los punteros a cada token
 * resultante se almacenan en el arreglo `campos` y el número de
 * tokens encontrados se pone en `*numCampos`.
 */

void split(char *linea, char *campos[], int *numCampos, const char *sep) {
    char *token = strtok(linea, sep);
    int i = 0;

    while (token != NULL) {
        campos[i++] = token;
        token = strtok(NULL, sep);
    }

    *numCampos = i;
}
/**
 * Carga artículos desde un archivo de texto al arreglo de estructuras.
 *
 * Esta función lee un archivo de texto donde cada línea representa un artículo.
 * Los campos dentro de cada línea deben venir separados por el caracter '|'.
 * Los campos esperados son, en este orden:
 *  - Nombre del autor
 *  - Apellido del autor
 *  - Título del artículo
 *  - Ruta/NOMBRE del archivo
 *  - Año de publicación
 *  - Resumen
 */
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
/**
 * Obtiene el nombre de archivo (basename) a partir de una ruta.
 *
 * Retorna un puntero al segmento final de la ruta después del último
 * '/' o '\', permitiendo comparar rutas usando solo el nombre del archivo.
 */
static const char *basename_from_path(const char *path) {
    if (path == NULL) return "";
    const char *p1 = strrchr(path, '/');
    const char *p2 = strrchr(path, '\\'); // por si hay rutas tipo Windows
    const char *b = p1 > p2 ? p1 : p2;
    return b ? b + 1 : path;
}

/**
 * Muestra en pantalla los artículos almacenados en un arreglo.
 *
 * Imprime en consola los datos principales de cada artículo: título, resumen
 * y ruta del archivo.  
 * Si se proporciona un arreglo de apariciones (listaApariciones), también
 * muestra cuántas veces aparece una palabra en cada artículo.
 */

void mostrarArticulos(Articulo lista[], int n, int *listaApariciones) {
    printf("\033[1;36m\n====================================\n");
    printf("           RESULTADOS\n");
    printf("====================================\033[0m\n");

    for (int i = 0; i < n; i++) {

        printf("\033[1;33m\n┌───────────────────────────────────┐\n");
        printf("│   Artículo %d\n", i + 1);
        printf("└───────────────────────────────────┘\033[0m\n");

        printf("\033[1;32mTítulo:      \033[0m%s\n", lista[i].titulo);
        printf("\033[1;32mResumen:     \033[0m%s\n", lista[i].resumen);
        printf("\033[1;32mRuta:        \033[0m%s\n", lista[i].ruta);

        if (listaApariciones != NULL) {
            printf("\033[1;32mApariciones: \033[0m%d\n", listaApariciones[i]);
        }
    }

    printf("\033[1;36m\n====================================\n");
    printf("              FIN\n");
    printf("====================================\033[0m\n");
}



//-------------------------HEAP TEXTUAL-------------------------
/**
 * Reorganiza un subárbol para mantener la propiedad de heap máximo
 *        usando un comparador textual.
 *
 * Esta función aplica la operación *heapify* sobre un índice específico
 * dentro del arreglo de artículos. Compara el nodo padre con sus dos hijos
 * utilizando la función de comparación textual indicada por el usuario
 * (por ejemplo: comparar por título, por ruta, etc.).  
 *
 * Si alguno de los hijos es "mayor" según el comparador, se intercambia con
 * el nodo actual y se continúa recursivamente sobre la posición afectada.
 */

void heapifyTexto(Articulo arr[], int n, int i, ComparadorTexto cmp) {

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
/**
 * Construye un heap máximo completo a partir de un arreglo de artículos,
 *        usando un comparador textual genérico.
 *
 * Recorre todos los nodos internos del arreglo (desde el último nodo no-hoja
 * hasta la raíz) aplicando la función `heapifyTexto()` con el comparador
 * recibido.  
 *
 * Con esto garantiza que el arreglo queda reorganizado en un **heap máximo**,
 * donde el elemento "mayor" según el comparador textual queda en la posición 0.
 *
 */

void construirHeapTexto(Articulo arr[], int n, ComparadorTexto cmp) {
    for (int i = n/2 - 1; i >= 0; i--) {
        heapifyTexto(arr, n, i, cmp);
    }
}

//-------------------------HEAP NUMERICO-------------------------

/**
 *  Restaura la propiedad de heap máximo en un subárbol numérico.
 *
 * Esta función compara un nodo con sus dos hijos (si existen) usando un
 * criterio **numérico**, el cual se obtiene mediante `obtenerClaveNumerica()`
 * aplicado a cada artículo.  
 *
 * Identifica cuál de los tres valores (padre, hijo izquierdo o hijo derecho)
 * es el mayor y, si el mayor no es la raíz del subárbol, intercambia los
 * elementos y aplica la función recursivamente en la posición afectada.
 *
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


/**
 * Construye un heap máximo basado en un criterio numérico.
 *
 * Esta función transforma un arreglo de artículos en un **heap máximo**
 * utilizando valores numéricos obtenidos mediante `obtenerClaveNumerica()`.
 * 
 * Recorre todos los nodos internos del arreglo (desde el último hacia la raíz)
 * y aplica `heapifyNumerico()` sobre cada uno, garantizando que el árbol
 * completo quede organizado bajo la propiedad del heap.
 *
 */

void construirHeapNumerico(Articulo arr[], int n) {
    // Desde el ultimo nodo interno hacia la raíz
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapifyNumerico(arr, n, i);
    }
}

/**
 * Aplica la operación de heapify sobre un heap numérico paralelo
 *        (apariciones + artículos).
 *
 * Esta función mantiene la propiedad de **heap máximo** sobre un arreglo de
 * enteros (`apariciones[]`), donde cada valor representa cuántas veces aparece
 * una palabra en un artículo.  
 *
 * Dado que cada posición del arreglo `apariciones[]` corresponde al artículo
 * en la misma posición de `lista[]`, la función también intercambia los
 * elementos del arreglo de artículos para preservar la alineación entre ambos.
 *
 */


void heapifyNumericoApariciones(int apariciones[], Articulo lista[], int n, int i) {

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
/**
 *  Construye un heap máximo basado en el arreglo de apariciones
 *        manteniendo alineado el arreglo de artículos.
 *
 * Esta función convierte el arreglo `apariciones[]` en un **montículo máximo**
 * (max-heap), llamando a `heapifyNumericoApariciones()` desde el último nodo
 * interno hasta la raíz.  
 *
 * Debido a que cada valor de `apariciones[]` corresponde al artículo en la
 * misma posición de `lista[]`, esta función también preserva la estructura
 * paralela entre ambos arreglos.
 *
 */

void construirHeapApariciones(int apariciones[], Articulo lista[], int n) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapifyNumericoApariciones(apariciones, lista, n, i);
    }
}



//-------------------------ORDENAMIENTO 1(TITULO)-------------------------
/**
 * Compara dos artículos alfabéticamente por su título.
 *
 * Esta función actúa como comparador textual para el heap sort.
 * Utiliza `strcmp()` para comparar los títulos de dos artículos,
 * permitiendo definir el criterio de ordenamiento basado en el
 * orden lexicográfico estándar.
 *
 */
int compararPorTitulo(const Articulo *a, const Articulo *b) {
    return strcmp(a->titulo, b->titulo);
}
/**
 *  Intercambia dos artículos dentro del arreglo.
 *
 * Esta función realiza un swap clásico entre dos estructuras
 * `Articulo`, preservando todos sus campos. Se usa en las
 * operaciones de heap tanto numéricas como textuales.
 *
 */
void intercambiar(Articulo *a, Articulo *b) {
    Articulo temp = *a;
    *a = *b;
    *b = temp;
}

/**
 *  Ordena un arreglo de artículos según su título
 *        utilizando el algoritmo Heap Sort (heap máximo textual).
 *
 * Esta función crea un heap máximo basado en los títulos de los
 * artículos, usando el comparador `compararPorTitulo()`. Luego
 * aplica heap sort extrayendo el máximo repetidamente para obtener
 * un arreglo ordenado de manera ascendente.
 *
 */
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
/**
 * Cuenta la cantidad de palabras presentes en un texto.
 *
 * Esta función recorre una cadena y contabiliza cuántas palabras contiene.
 * Una palabra se define como una secuencia continua de caracteres no-espacio,
 * separada por uno o más espacios.
 *
 */
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
/**
 *  Obtiene un valor numérico asociado a un artículo:
 *        la cantidad de palabras presentes en su título.
 *
 * Esta función sirve como clave de comparación para el heap numérico,
 * permitiendo ordenar artículos basados en el tamaño de su título.
 */
int obtenerClaveNumerica(const Articulo *a) {
    return contarPalabras(a->titulo);
}
/**
 *  Ordena artículos según la cantidad de palabras en su título
 *        utilizando Heap Sort numérico.
 *
 * Construye un heap máximo basado en la cantidad de palabras del título
 * de cada artículo, y luego aplica Heap Sort para producir un arreglo
 * ordenado de manera ascendente.
 *
 */
void heapSortPorTamTitulo(Articulo arr[], int n) {
    // construir heap basado en cantidad de palabras
    construirHeapNumerico(arr, n);

    for (int i = n - 1; i > 0; i--) {
        intercambiar(&arr[0], &arr[i]);
        heapifyNumerico(arr, i, 0);
    }
}
//-------------------------ORDENAMIENTO 3(Nombre del archivo)-------------------------
/**
 *  Compara dos artículos según su ruta de archivo.
 *
 * Esta función realiza una comparación lexicográfica entre las rutas
 * de dos artículos usando `strcmp()`. Se utiliza como función de
 * comparación para las operaciones de heap textual.
 */
int compararPorRuta(const Articulo *a, const Articulo *b) {
    const char *na = basename_from_path(a->ruta);
    const char *nb = basename_from_path(b->ruta);
    return strcmp(na, nb);
}
/**
 *  Ordena artículos según su ruta de archivo utilizando Heap Sort.
 *
 * Construye un heap máximo basado en la comparación textual de rutas,
 * y luego ordena el arreglo en orden ascendente mediante Heap Sort.
 *
 */
void heapSortPorRuta(Articulo arr[], int n) {
    construirHeapTexto(arr, n, compararPorRuta);

    for (int i = n - 1; i > 0; i--) {
        intercambiar(&arr[0], &arr[i]);
        heapifyTexto(arr, i, 0, compararPorRuta);
    }
}

//------------------------ORDENAMIENTO 4(Apariciones de palabra)------------------------

/**
 *  Convierte una cadena a minúsculas en el mismo buffer.
 *
 * Recorre la cadena modificando cada carácter a su forma minúscula
 * usando `tolower()`. Se utiliza como ayuda para normalizar texto
 * antes de realizar comparaciones insensibles a mayúsculas/minúsculas.
 *
 */
static void to_lower_inplace(char *s) {
    for (int i = 0; s[i]; i++) s[i] = (char)tolower((unsigned char)s[i]);
}

/**
 * Cuenta apariciones de una palabra dentro de un texto (insensible a mayúsculas).
 *
 * Esta función:
 * - Convierte copias del texto y la palabra a minúsculas.
 * - Busca ocurrencias mediante `strstr()`.
 * - Valida que la coincidencia sea una **palabra completa**, verificando
 *   que a izquierda y derecha no haya caracteres alfanuméricos.
 *
 */
int contarApariciones(const char *texto, const char *palabra) {
    if (!texto || !palabra) return 0;
    int lenp = strlen(palabra);
    if (lenp == 0) return 0;

    // Hacemos copias locales en minúscula (seguras)
    char *t_copy = strdup(texto);
    char *p_copy = strdup(palabra);
    if (!t_copy || !p_copy) {
        free(t_copy); free(p_copy);
        return 0;
    }
    to_lower_inplace(t_copy);
    to_lower_inplace(p_copy);

    int contador = 0;
    char *p = t_copy;
    while ((p = strstr(p, p_copy)) != NULL) {
        // Verificar limites izquierdo y derecho para que sea palabra completa
        int ok_left = (p == t_copy) || !isalnum((unsigned char)p[-1]);
        char *p_right = p + lenp;
        int ok_right = (*p_right == '\0') || !isalnum((unsigned char)*p_right);

        if (ok_left && ok_right) contador++;

        // avanzar el puntero para seguir buscando (evita bucle infinito)
        p = p + lenp;
    }

    free(t_copy);
    free(p_copy);
    return contador;
}


/**
 * Invierte dos arreglos paralelos manteniendo su alineación.
 *
 * Esta función es un helper utilizado después del heap sort, ya que
 * el heap genera un orden descendente. Invertir ambos arreglos deja
 * los datos en **orden ascendente**, manteniendo sincronizados:
 *
 * - El arreglo de apariciones.
 * - El arreglo de artículos.
 *
 */
static void reverse_arrays(int apar[], Articulo lista[], int n) {
    for (int i = 0, j = n - 1; i < j; i++, j--) {
        // swap apariciones
        int tmp = apar[i]; apar[i] = apar[j]; apar[j] = tmp;
        // swap articulos
        intercambiar(&lista[i], &lista[j]);
    }
}
/**
 * Consulta una palabra ingresada por el usuario y ordena los artículos
 *        por la cantidad de apariciones en título + resumen.
 *
 * Flujo completo:
 * 1. Solicita al usuario una palabra (con limpieza del buffer de entrada).
 * 2. Usa `contarApariciones()` para contar ocurrencias en título y resumen.
 * 3. Construye un heap máximo con `construirHeapApariciones()`.
 * 4. Aplica un heap sort que:
 *    - Reordena el arreglo de apariciones.
 *    - Mantiene alineado el arreglo de artículos.
 * 5. Invierte ambos arreglos para obtener un orden ascendente.
 *

 */
int* consultarPalabra(Articulo lista[], int cantidad) {
    // limpiar buffer antes de fgets (por si viene un scanf previa)
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);

    char palabra_buf[128];
    printf("\nDigite la palabra que desea buscar: ");
    if (fgets(palabra_buf, sizeof(palabra_buf), stdin) == NULL) {
        return NULL;
    }
    palabra_buf[strcspn(palabra_buf, "\n")] = '\0';
    if (palabra_buf[0] == '\0') {
        printf("Palabra vacía, abortando búsqueda.\n");
        return NULL;
    }

    int *listaApariciones = calloc(cantidad, sizeof(int));
    if (!listaApariciones) {
        perror("calloc");
        return NULL;
    }

    // Contar en título + resumen
    for (int i = 0; i < cantidad; i++) {
        int c1 = contarApariciones(lista[i].titulo, palabra_buf);
        int c2 = contarApariciones(lista[i].resumen, palabra_buf);
        listaApariciones[i] = c1 + c2; // suma apariciones en titulo+resumen
    }

    // Construir heap usando apariciones
    construirHeapApariciones(listaApariciones, lista, cantidad);

    // heap sort (
    for (int i = cantidad - 1; i > 0; i--) {
        // intercambiar apariciones
        int tmp = listaApariciones[0];
        listaApariciones[0] = listaApariciones[i];
        listaApariciones[i] = tmp;

        // se intercambian articulos
        intercambiar(&lista[0], &lista[i]);
        heapifyNumericoApariciones(listaApariciones, lista, i, 0);
    }

    reverse_arrays(listaApariciones, lista, cantidad);

    printf("\nArtículos ordenados por cantidad de apariciones de la palabra '%s':\n", palabra_buf);
    return listaApariciones;
}




void printAscii(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("[No se pudo cargar ASCII desde %s]\n", filename);
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }
    fclose(f);
    getchar();
}


int main() {
    Articulo lista[MAX_ARTICULOS];
    int cantidad = cargarIndice(RUTA, lista, MAX_ARTICULOS);

    int opcion;
    int mostrar;
    int *listaApariciones = NULL;

    if (cantidad == 0) {
        printf("No se cargaron artículos.\n");
        return 0;
    }
    printAscii("ascii.txt");
    printf("Cargados: %d artículos\n", cantidad);

    do {

        

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
                if (listaApariciones != NULL) {
                    free(listaApariciones);
                }
                listaApariciones = consultarPalabra(lista, cantidad);
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
            while (scanf("%d", &mostrar) != 1) {
                printf("\nError, escribe un valor válido por favor (1-%d)\n", cantidad);
                scanf("%*s");
            }
            if (mostrar >= 1 && mostrar <= cantidad) {
                mostrarArticulos(lista, mostrar, listaApariciones);
                if (listaApariciones != NULL) {
                    free(listaApariciones);
                    listaApariciones = NULL;
                }
            }
        }

    } while (opcion != 5);

    return 0;
}

