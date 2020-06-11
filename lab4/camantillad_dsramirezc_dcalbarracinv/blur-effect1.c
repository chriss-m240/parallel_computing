#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <mpi.h>
#include "omp.h"

int max(int a,int b){
	if(a>b)
		return a;
	return b;
}
int min(int a,int b){
	if(a<b)
		return a;
	return b;
}
struct param_struct {
	int *scl;
	int *tcl;
	int w;
	int h;
	int r;
	int start;
	int end;
	int id;
};

int kernel_size, nThreads;
int numprocs, processId;

void boxesForGauss(int sigma, int n, double *bxs) {
    double wIdeal = sqrt((12*sigma*sigma/n)+1);
    double wl = floor(wIdeal);
		if(fmod(wl, 2.0) == 0.0)
			wl--;
    double wu = wl+2;

    double mIdeal = (12*sigma*sigma - n*wl*wl - 4*n*wl - 3*n)/(-4*wl - 4);
    double m = round(mIdeal);

		for (int i = 0; i < n; i++) {
			*(bxs+i) = (i<m?wl:wu);
		}
}

void boxBlurH(int *scl, int *tcl, int w, int h, int r, int start, int end) {
    for(int i=start; i<min(h, end); i++)
        for(int j=0; j<w; j++)  {
            int val = 0;
            for(int ix=j-r; ix<j+r+1; ix++) {
                int x = min(w-1, max(0, ix));
                val += *(scl+(i*w+x));
            }
            *(tcl+(i*w+j)) = val/(r+r+1);
        }
}

void boxBlurT(int *scl, int *tcl, int w, int h, int r, int start, int end) {
    for(int i=start; i<min(h, end); i++)
        for(int j=0; j<w; j++) {
            int val = 0;
            for(int iy=i-r; iy<i+r+1; iy++) {
                int y = min(h-1, max(0, iy));
                val += *(scl + (y * w + j));
            }
            *(tcl+(i*w+j)) = val/(r+r+1);
        }
}


void boxBlur(int *scl, int *tcl, int w, int h, int r, int start, int end) {
		for (int i = 0; i < (w * h); i++) {
				int aux = *(scl + i);
				*(tcl + i) = aux;
		}
    boxBlurH(tcl, scl, w, h, r, start, end);
    boxBlurT(scl, tcl, w, h, r, start, end);
}

void *gaussBlur(void *arg) {
    struct param_struct *data;
	  data = (struct param_struct *)arg;
    double *bxs = (double *)malloc(sizeof(double) * 3);
		boxesForGauss(data->r, 3, bxs);

    boxBlur(data->scl, data->tcl, data->w, data->h, (int)((*(bxs)-1)/2), data->start, data->end);
   // boxBlur(data->tcl, data->scl, data->w, data->h, (int)((*(bxs+1)-1)/2), data->start, data->end);
   // boxBlur(data->scl, data->tcl, data->w, data->h, (int)((*(bxs+2)-1)/2), data->start, data->end);

    return 0;
}

void parallelize(int *scl, int *tcl, int w, int h, int r, int processId) {
	int dh = h/(nThreads*numprocs);
	struct param_struct params_arr[nThreads];

	#pragma omp parallel num_threads(nThreads)
	{
		int threadId = omp_get_thread_num();
		int GLOBAL_ID = (processId * (nThreads)) + threadId;
		int ID = omp_get_thread_num();
		struct param_struct params;
	//	printf("processId: %d threadStart: %d threadEnd: %d pro %d\n", GLOBAL_ID, nThreads, threadId,processId);
		params.scl = scl;
		params.tcl = tcl;
		params.w = w;
		params.h = h;
		params.r = r;
		params.start =GLOBAL_ID * dh;
		params.end = ((GLOBAL_ID + 1) * dh);
		params.id = ID;
		params_arr[ID] = params;
		gaussBlur(&params_arr[ID]);
	}
}


int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &processId);

	int width, height, n_channels;
	unsigned char *img = stbi_load(argv[1], &width, &height, &n_channels, 0);

	int img_size = width * height;
	int *int_img = (int *)malloc(sizeof(int) * (3*img_size));
	int *r_ch = (int *)malloc(sizeof(int) * (img_size));
	int *g_ch = (int *)malloc(sizeof(int) * (img_size));
	int *b_ch = (int *)malloc(sizeof(int) * (img_size));

	int offset = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

				*(r_ch+(i*width)+j) = (int)*(img+offset);
				*(g_ch+(i*width)+j) = (int)*(img+offset+1);
				*(b_ch+(i*width)+j) = (int)*(img+offset+2);

				offset += n_channels;
		}
	}
	kernel_size=atoi(argv[3]);
	nThreads=atoi(argv[4]);

	//if (processId == 0)
		//printf("\nLaunching with %i processes\n", numprocs);

	int dh = height/numprocs;
	int processStart = processId * dh;
	int processEnd = ((processId + 1) * dh);
	
	//printf("processStart: %d processEnd %d\n", processStart, processEnd);



	int *r_target_ch = (int *)malloc(sizeof(int) * (img_size));
	int *g_target_ch = (int *)malloc(sizeof(int) * (img_size));
	int *b_target_ch = (int *)malloc(sizeof(int) * (img_size));

	parallelize(r_ch, r_target_ch, width, height, kernel_size, processId);
	parallelize(g_ch, g_target_ch, width, height, kernel_size, processId);
	parallelize(b_ch, b_target_ch, width, height, kernel_size, processId);



	offset = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (processStart <= i && i < processEnd) {
				int_img[offset] = *(r_target_ch + (i*width)+j);
				int_img[offset+1] = *(g_target_ch + (i*width)+j);
				int_img[offset+2] = *(b_target_ch + (i*width)+j);
			} 
				offset += n_channels;
		}
	}

	int *int_final_img = (int *)malloc(sizeof(int) * (3*img_size));
	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(int_img, int_final_img, (3*img_size), MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (processId == 0) {
		for (int i = 0; i < 3*img_size; i++) {
			img[i] = int_final_img[i];
		}

		stbi_write_jpg(argv[2], width, height, n_channels, img, 50);
	}
	
	free(r_target_ch);
	free(g_target_ch);
	free(b_target_ch);
	free(r_ch);
	free(g_ch);
	free(b_ch);

	MPI_Finalize();

	return 0;
}
