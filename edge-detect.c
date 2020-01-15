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

typedef struct stack_t {
    char** path;
    int count;
    int max;
} Stack;

static pthread_mutex_t mutex;
static pthread_cond_t is_img_ok;
static Stack stack;

struct Img stackImg[50000];
struct Img openImage(char *path);

void *producer(void *img);
void *consumer();
void *apply_effect(void *img);


void *producer(void *img)
{
    printf("producer: stack.count: %d\n", stack.max);
    pthread_mutex_lock(&mutex);

    char* imgPath = (char *)img;
    char path [200] = "out/";
	struct Img imgToPush = openImage(imgPath);

    strcat(path, imgPath);
	imgToPush.name = malloc(sizeof(char) * strlen(path));
	strcpy(imgToPush.name, path);

	apply_effect(&imgToPush);
	push(imgToPush, stackImg);
	pthread_cond_signal(&is_img_ok);

	pthread_mutex_unlock(&mutex);
	return NULL;
}

void *consumer()
{
    printf("stack: %d\n", stack.max);
	while(1) {
        pthread_mutex_lock(&mutex);
        while(isempty()) {
            pthread_cond_wait(&is_img_ok, &mutex);
        }
        struct Img img = pop(stackImg);
        int w = img.original.bmp_header.width;

        save_bitmap(img.new_i, img.name);
        stack.max--;

        if(stack.max == 0) {
            break;
        }
        pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

struct Img openImage(char *path)
{
	struct Img Img;
	Img.original = open_bitmap(path);
	return Img;
}

void fillStackImage() {
    DIR *d;
    struct dirent *dir;
    int counter = 0;

    stack.path = (char *)malloc(sizeof(char*));
    stack.count = 0;

    d = opendir("image");
    while ((dir = readdir(d)) != NULL)
    {
        if (strstr(dir->d_name, ".bmp") != NULL)
        {
            stack.path = realloc(stack.path, sizeof(char*) * counter + 1);
            stack.path[counter] = malloc(255 * sizeof(char));
            strcpy(stack.path[counter], "image/");
            strcat(stack.path[counter], dir->d_name);
            stack.count++;
            counter++;
        }
    }

    stack.max = stack.count;
}

int main(int argc, char **argv)
{
	int counter = 0;
	int numberThreads = 3;
    char imagePath[numberThreads][255];

    pthread_t threads[numberThreads];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_cond_init(&is_img_ok, NULL);
	pthread_mutex_init(&mutex, NULL);

	fillStackImage();
	printf("stack.count: %d\n", stack.max);

    while(stack.count > 0) {
        pthread_create(&threads[counter], &attr, producer, stack.path[stack.count - 1]);
        stack.count--;

        counter++;

        if(counter == numberThreads - 1) {
            counter = 0;
        }
    }

	pthread_create(&threads[1], NULL, consumer, NULL);
	pthread_join(threads[1], NULL);
	pthread_cond_destroy(&is_img_ok);
	pthread_mutex_destroy(&mutex);
	return 0;
}

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