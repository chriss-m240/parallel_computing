#include <bits/stdc++.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "omp.h"

using namespace std;

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

double* boxesForGauss(int sigma, int n) {
    double wIdeal = sqrt((12*sigma*sigma/n)+1);
    double wl = floor(wIdeal);
		if(fmod(wl, 2.0) == 0.0)
			wl--;
    double wu = wl+2;

    double mIdeal = (12*sigma*sigma - n*wl*wl - 4*n*wl - 3*n)/(-4*wl - 4);
    double m = round(mIdeal);
		double *sizes = (double *)malloc(sizeof(double) * 3);;
		for (int i = 0; i < n; i++) {
			*(sizes+i) = (i<m?wl:wu);
		}

		return sizes;
}

void boxBlurH(int *scl, int *tcl, int w, int h, int r, int start, int end) {
    for(int i=start; i<end; i++)
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
    for(int i=start; i<end; i++)
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
    double *bxs = boxesForGauss(data->r, 3);

    boxBlur(data->scl, data->tcl, data->w, data->h, (int)((*(bxs)-1)/2), data->start, data->end);
    boxBlur(data->tcl, data->scl, data->w, data->h, (int)((*(bxs+1)-1)/2), data->start, data->end);
    boxBlur(data->scl, data->tcl, data->w, data->h, (int)((*(bxs+2)-1)/2), data->start, data->end);

    return 0;
}

void parallelize(int *scl, int *tcl, int w, int h, int r) {
	int dh = h/nThreads;
	struct param_struct params_arr[nThreads];

	#pragma omp parallel num_threads(nThreads)
	{
		int ID = omp_get_thread_num();
		struct param_struct params;
		params.scl = scl;
		params.tcl = tcl;
		params.w = w;
		params.h = h;
		params.r = r;
		params.start = ID * dh;
		params.end = (ID + 1) * dh;
		params.id = ID;
		params_arr[ID] = params;
		gaussBlur(&params_arr[ID]);
	}
}

int main(int argc, char *argv[]){
	kernel_size=atoi(argv[3]);
	nThreads=atoi(argv[4]);
	int width, height, n_channels;
	unsigned char *img = stbi_load(argv[1], &width, &height, &n_channels, 0);

	int img_size = width * height;
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

	int *r_target_ch = (int *)malloc(sizeof(int) * (img_size));
	int *g_target_ch = (int *)malloc(sizeof(int) * (img_size));
	int *b_target_ch = (int *)malloc(sizeof(int) * (img_size));

	parallelize(r_ch, r_target_ch, width, height, kernel_size);
	parallelize(g_ch, g_target_ch, width, height, kernel_size);
	parallelize(b_ch, b_target_ch, width, height, kernel_size);

	offset = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			img[offset] = *(r_target_ch + (i*width)+j);
			img[offset+1] = *(g_target_ch + (i*width)+j);
			img[offset+2] = *(b_target_ch + (i*width)+j);
			offset += n_channels;
		}
	}

	stbi_write_jpg(argv[2], width, height, n_channels, img, 50);
	free(r_target_ch);
	free(g_target_ch);
	free(b_target_ch);
	free(r_ch);
	free(g_ch);
	free(b_ch);

	return 0;
}
