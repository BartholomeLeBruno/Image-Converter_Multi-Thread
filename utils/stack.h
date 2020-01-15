//
// Created by alexis on 09/01/20.
//

#ifndef IMAGE_CONVERTER_MULTI_THREAD_STACK_H
#define IMAGE_CONVERTER_MULTI_THREAD_STACK_H

#include <stdio.h>
#include "../bitmap.h"

typedef struct Img
{
	Image original;
	Image new_i;
	char* name;
} Img;


int isempty();

int isfull();

struct Img peek(struct Img* stack);

struct Img pop(struct Img* stack);

extern void push(struct Img data, struct Img* stack);

#endif //IMAGE_CONVERTER_MULTI_THREAD_STACK_H
