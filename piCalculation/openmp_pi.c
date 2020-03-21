#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "omp.h"

#define ITERATIONS 2e09

void calculatePi(double *piTotal, int ID, int nThreads) {
    int initIteration, endIteration;

    initIteration = (ITERATIONS/nThreads) * ID;
    endIteration = initIteration + ((ITERATIONS/nThreads) *(ID + 1));
    piTotal[ID] = 0.0;
    do{
        piTotal[ID] = piTotal[ID] + (double)(4.0 / ((initIteration*2)+1));
        initIteration++;
        piTotal[ID] = piTotal[ID] - (double)(4.0 / ((initIteration*2)+1));
        initIteration++;
    } while(initIteration < endIteration);
}

int main(int argc, char *argv[]){
    int i;
    int nThreads = *argv[1]-'0';
    double piTotal[nThreads];


    // printf("With %d threads, ",nThreads);

    #pragma omp parallel num_threads(nThreads)
    {
      int ID = omp_get_thread_num();
      calculatePi(piTotal, ID, nThreads);
    }

    for(i = 1; i < nThreads; i++) {
        piTotal[0] = piTotal[0] + piTotal[i];
    }

    // printf("pi: %2.12f \n", piTotal[0]);
}
