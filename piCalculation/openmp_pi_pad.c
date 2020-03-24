#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "omp.h"

#define ITERATIONS 2e09
#define PAD 8

void calculatePi(double *piTotal, int ID, int nThreads) {
    long long initIteration, endIteration;

    initIteration = (ITERATIONS/nThreads) * ID;
    endIteration = ((ITERATIONS/nThreads) *(ID + 1)) -1;
    piTotal[ID*PAD] = 0.0;
    do{
        piTotal[ID*PAD] = piTotal[ID*PAD] + (double)(4.0 / ((initIteration*2)+1));
        initIteration++;
        piTotal[ID*PAD] = piTotal[ID*PAD] - (double)(4.0 / ((initIteration*2)+1));
        initIteration++;
    } while(initIteration < endIteration);
}

int main(int argc, char *argv[]){
    int i;
    int nThreads = *argv[1]-'0';
    double piTotal[nThreads*PAD];


    printf("With %d threads, ",nThreads);

    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);

    #pragma omp parallel num_threads(nThreads)
    {
      int ID = omp_get_thread_num();
      calculatePi(piTotal, ID, nThreads);
    }

    for(i = 1; i < nThreads; i++) {
        piTotal[0] = piTotal[0] + piTotal[i*PAD];
    }

    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    printf("Time elapsed: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);

    printf("pi: %2.12f \n", piTotal[0]);
}
