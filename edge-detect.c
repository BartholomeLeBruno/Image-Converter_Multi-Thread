/*
	//gcc edge-detect.c bitmap.c -O2 -ftree-vectorize -fopt-info -mavx2 -fopt-info-vec-all -pthread
	//UTILISER UNIQUEMENT DES BMP 24bits
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils/stack.h"
#include "bitmap.h"
#include <stdint.h>
#include <pthread.h>
#include <dirent.h>
#include <string.h>

#define DIM 3
#define LENGHT DIM
#define OFFSET (DIM / 2)

const float KERNEL[DIM][DIM] = {{-1, -1, -1},
								{-1, 8, -1},
								{-1, -1, -1}};

struct Img stackImg[50000];
static pthread_cond_t is_imgOK;
static pthread_mutex_t mutex;
struct Img openImage(char *path);
void *producer(void *img);
void *consumer();

void *apply_effect(void *img);
void *apply_effect(void *img)
{

	int w = ((struct Img *)img)->original.bmp_header.width;
	int h = ((struct Img *)img)->original.bmp_header.height;

	((struct Img *)img)->new_i = new_image(w, h, ((struct Img *)img)->original.bmp_header.bit_per_pixel, ((struct Img *)img)->original.bmp_header.color_planes);

	for (int y = OFFSET; y < h - OFFSET; y++)
	{
		for (int x = OFFSET; x < w - OFFSET; x++)
		{
			Color_e c = {.Red = 0, .Green = 0, .Blue = 0};

			for (int a = 0; a < LENGHT; a++)
			{
				for (int b = 0; b < LENGHT; b++)
				{
					int xn = x + a - OFFSET;
					int yn = y + b - OFFSET;

					Pixel *p = &(((struct Img *)img)->original).pixel_data[yn][xn];

					c.Red += ((float)p->r) * KERNEL[a][b];
					c.Green += ((float)p->g) * KERNEL[a][b];
					c.Blue += ((float)p->b) * KERNEL[a][b];
				}
			}

			Pixel *dest = &(((struct Img *)img)->new_i).pixel_data[y][x];
			dest->r = (uint8_t)(c.Red <= 0 ? 0 : c.Red >= 255 ? 255 : c.Red);
			dest->g = (uint8_t)(c.Green <= 0 ? 0 : c.Green >= 255 ? 255 : c.Green);
			dest->b = (uint8_t)(c.Blue <= 0 ? 0 : c.Blue >= 255 ? 255 : c.Blue);
		}
	}
}

void *producer(void *img)
{
	struct Img imgToPush = openImage((char *)img);
	pthread_mutex_lock(&mutex);
	apply_effect(&imgToPush);
	push(imgToPush, stackImg);
	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&is_imgOK);
}

void *consumer()
{
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&is_imgOK, &mutex);
	struct Img img = pop(stackImg);
    int w = img.original.bmp_header.width;
	save_bitmap(img.new_i, "voila.bmp");
	pthread_mutex_unlock(&mutex);
}
struct Img openImage(char *path)
{
	struct Img Img;
	Img.original = open_bitmap(path);
	return Img;
}

int main(int argc, char **argv)
{
	DIR *d;
	pthread_t threads[2];
	struct dirent *dir;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_cond_init(&is_imgOK, NULL);
	pthread_mutex_init(&mutex, NULL);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	d = opendir("image");
	char imagePath[255] = "image/";
	for (int i = 0; i < 1; i++)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (strstr(dir->d_name, ".bmp") != NULL)
			{
				strcat(imagePath, dir->d_name);
				pthread_create(&threads[i], &attr, producer, imagePath);
				break;
			}
		}
	}

	pthread_create(&threads[1], NULL, consumer, NULL);
	//pthread_create(&a, NULL, apply_effect, (void *)Img);
	pthread_join(threads[1], NULL);
	pthread_cond_destroy(&is_imgOK);
	pthread_mutex_destroy(&mutex);
	return 0;
}