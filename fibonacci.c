#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

/* Tipo de dato usado para almacenar elementos de Fibonacci */
typedef unsigned long long ull;

/* Estructura para pasar datos al hilo trabajador */
typedef struct {
    ull *array;  /* arreglo compartido */
    int N;       /* cantidad de elementos a generar */
} fib_data;

/* Prototipos */
void* CalcFibonacci_for_thread(void *arg);

int main(int argc, char **argv) {
    int N;

    /* Validación de argumentos */
    if (argc != 2) {
        printf("\nIncorrect format, use: ./fibonacci <N>\n");
        return 1;
    }

    N = atoi(argv[1]);

    if (N < 0) {
        printf("\nN must be >= 0.\n");
        return 1;
    }

    /* Reserva de memoria para el arreglo compartido */
    ull *fib_array = (ull*) malloc(sizeof(ull) * (N == 0 ? 1 : N));
    if (fib_array == NULL) {
        printf("\nError allocating memory.\n");
        return 1;
    }

    /* Advertencia por posible overflow */
    if (N > 94) {
        printf("\nWARNING: For N > 94, Fibonacci values overflow unsigned long long.\n");
    }

    /* Preparar estructura de datos */
    fib_data data;
    data.array = fib_array;
    data.N = N;

    pthread_t thread;


    /* Crear hilo trabajador */
    pthread_create(&thread, NULL, CalcFibonacci_for_thread, (void*)&data);

    /* Esperar finalización */
    pthread_join(thread, NULL);

    /* Imprimir resultados */
    printf("Secuencia Fibonacci (N = %d):\n", N);
    for (int i = 0; i < N; i++) {
        printf("%llu", fib_array[i]);
        if (i + 1 < N) printf(" ");
    }
    printf("\n");

    free(fib_array);
    return 0;
}

/* Función ejecutada por el hilo */
void* CalcFibonacci_for_thread(void *arg) {
    fib_data *data = (fib_data*) arg;
    ull *arr = data->array;
    int N = data->N;

    if (N == 0) pthread_exit(NULL);

    arr[0] = 0ULL;
    if (N == 1) pthread_exit(NULL);

    arr[1] = 1ULL;

    for (int i = 2; i < N; i++) {
        arr[i] = arr[i-1] + arr[i-2];
    }

    pthread_exit(NULL);
}
