#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

void* CalcPi_for_thread(void *arg);
double f(double a);
double GetTime();

typedef struct {
    int start;
    int end;
    double h;
    double partial_sum;
} th_data;

int main(int argc, char **argv) {
    int n = 2000000000;
    const double fPi25DT = 3.141592653589793238462643;
    int threads_number = atoi(argv[1]);
    double fTimeStart, fTimeEnd;
    double fPi;
    double total_sum = 0.0;

    if (argc != 2) {
        printf("\nIncorrect format, use: ./pi_p <threads_number>\n");
        return 1;
    }

    if (n <= 0 || n > 2147483647) {
        printf("\ngiven value has to be between 0 and 2147483647\n");
        return 1;
    }

    if (threads_number <= 0) {
        printf("\nthreads number must have greater than 0.\n");
        return 1;
    }

    pthread_t threads[threads_number];
    th_data data[threads_number];

    double fH = 1.0 / (double) n;
    int portion = n / threads_number;

    fTimeStart = GetTime();

    for (int t = 0; t < threads_number; t++) {
        data[t].start = t * portion;
        data[t].end = (t == threads_number - 1) ? n : (t + 1) * portion;
        data[t].h = fH;
        pthread_create(&threads[t], NULL, CalcPi_for_thread, (void*)&data[t]);
    }

    
    for (int t = 0; t < threads_number; t++) {
        pthread_join(threads[t], NULL);
        total_sum += data[t].partial_sum;
    }

    fPi = fH * total_sum;

    fTimeEnd = GetTime();

    printf("pi is approximately = %.20f\n", fPi);
    printf("Error = %.20f\n", fabs(fPi - fPi25DT));
    printf("Time was = %.4f s\n", fTimeEnd - fTimeStart);

    return 0;
}

double f(double a) {
    return (4.0 / (1.0 + a*a));
}

void* CalcPi_for_thread(void *arg) {
    th_data *data = (th_data*)arg;
    double fSum = 0.0;

    for (int i = data->start; i < data->end; i++) {
        double fX = data->h * ((double) i + 0.5);
        fSum += f(fX);
    }

    data->partial_sum = fSum;
    pthread_exit(NULL);
}

double GetTime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec * 1e-6;
}