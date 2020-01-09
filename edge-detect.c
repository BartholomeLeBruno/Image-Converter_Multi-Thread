/*
	//gcc edge-detect.c bitmap.c -O2 -ftree-vectorize -fopt-info -mavx2 -fopt-info-vec-all -pthread
	//UTILISER UNIQUEMENT DES BMP 24bits
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bitmap.h"
#include <stdint.h>
#include <pthread.h>

#define DIM 3
#define LENGHT DIM
#define OFFSET DIM /2

const float KERNEL[DIM][DIM] = {{-1, -1,-1},
							   {-1,8,-1},
							   {-1,-1,-1}};

typedef struct Color_t {
	float Red;
	float Green;
	float Blue;
} Color_e;

typedef struct Img {
    Image original;
    Image new_i;
} Img;

void* apply_effect(void * img);
void* apply_effect(void * img) {

	int w = ((struct Img*)img)->original.bmp_header.width;
	int h = ((struct Img*)img)->original.bmp_header.height;

    ((struct Img*)img)->new_i = new_image(w, h, ((struct Img*)img)->original.bmp_header.bit_per_pixel, ((struct Img*)img)->original.bmp_header.color_planes);

	for (int y = OFFSET; y < h - OFFSET; y++) {
		for (int x = OFFSET; x < w - OFFSET; x++) {
			Color_e c = { .Red = 0, .Green = 0, .Blue = 0};

			for(int a = 0; a < LENGHT; a++){
				for(int b = 0; b < LENGHT; b++){
					int xn = x + a - OFFSET;
					int yn = y + b - OFFSET;

					Pixel* p = &(((struct Img*)img)->original).pixel_data[yn][xn];

					c.Red += ((float) p->r) * KERNEL[a][b];
					c.Green += ((float) p->g) * KERNEL[a][b];
					c.Blue += ((float) p->b) * KERNEL[a][b];
				}
			}

			Pixel* dest = &(((struct Img*)img)->new_i).pixel_data[y][x];
			dest->r = (uint8_t)  (c.Red <= 0 ? 0 : c.Red >= 255 ? 255 : c.Red);
			dest->g = (uint8_t) (c.Green <= 0 ? 0 : c.Green >= 255 ? 255 : c.Green);
			dest->b = (uint8_t) (c.Blue <= 0 ? 0 : c.Blue >= 255 ? 255 : c.Blue);
		}
	}
}

int main(int argc, char** argv) {
    struct Img *Img = (struct Img *)malloc(sizeof(struct Img));
    pthread_t a;
    Img->original = open_bitmap("bmp_tank.bmp");
	pthread_create(&a, NULL, apply_effect, (void *)Img);
	pthread_join(a, NULL);
	save_bitmap(Img->new_i, "test_out.bmp");
	return 0;
}