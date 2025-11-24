/* fibonacci.c
   Compilar:
     gcc -o fibonacci fibonacci.c -lpthread

   Ejecutar ejemplo:
     ./fibonacci 10
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

/* Tipo para almacenar resultado: usamos unsigned long long (64-bit) */
typedef unsigned long long ull;

/* Estructura para pasar argumentos al hilo trabajador */
typedef struct {
    ull *array;   /* puntero al arreglo compartido (de tamaño N) */
    size_t N;     /* número de elementos a generar */
} fib_args_t;

/* Hilo trabajador: calcula los N elementos de Fibonacci y los guarda en array */
void *fib_worker(void *arg) {
    if (arg == NULL) return NULL;
    fib_args_t *args = (fib_args_t *)arg;
    ull *arr = args->array;
    size_t N = args->N;

    if (N == 0) {
        /* nada que hacer */
        return NULL;
    }

    /* f0 */
    arr[0] = 0ULL;
    if (N == 1) return NULL;

    /* f1 */
    arr[1] = 1ULL;
    for (size_t i = 2; i < N; ++i) {
        /* cuidado: posible overflow si N es muy grande */
        arr[i] = arr[i-1] + arr[i-2];
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s N\nGenera los primeros N números de Fibonacci (f0..f{N-1}).\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *endptr = NULL;
    errno = 0;
    long val = strtol(argv[1], &endptr, 10);
    if (errno != 0 || *endptr != '\0' || val < 0) {
        fprintf(stderr, "Argumento N inválido: '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

    size_t N = (size_t) val;
    /* Allocate shared array */
    ull *fib_array = (ull *) malloc(sizeof(ull) * (N == 0 ? 1 : N));
    if (fib_array == NULL && N != 0) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    /* Pre-check: advertencia sobre overflow potencial en 64 bits */
    if (N > 94) { /* F(94) = 19740274219868223167 fits in 64-bit unsigned, F(95) overflows */
        fprintf(stderr, "ADVERTENCIA: N = %zu puede causar overflow de unsigned long long (64-bit).\n"
                        "Considere usar una librería de enteros grandes (GMP) si necesita N mayor.\n",
                        N);
    }

    /* Preparar argumentos para el hilo */
    fib_args_t args;
    args.array = fib_array;
    args.N = N;

    pthread_t thread;
    int rc = pthread_create(&thread, NULL, fib_worker, &args);
    if (rc != 0) {
        fprintf(stderr, "Error creando hilo: %s\n", strerror(rc));
        free(fib_array);
        return EXIT_FAILURE;
    }

    /* Esperar la finalización del hilo trabajador */
    rc = pthread_join(thread, NULL);
    if (rc != 0) {
        fprintf(stderr, "Error en pthread_join: %s\n", strerror(rc));
        free(fib_array);
        return EXIT_FAILURE;
    }

    /* Imprimir los N valores (main accede a los resultados después de join) */
    printf("Fibonacci (N = %zu):\n", N);
    for (size_t i = 0; i < N; ++i) {
        printf("%llu", (unsigned long long) fib_array[i]);
        if (i + 1 < N) printf(" ");
    }
    printf("\n");

    free(fib_array);
    return EXIT_SUCCESS;
}
