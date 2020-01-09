//
// Created by alexis on 09/01/20.
//

#ifndef IMAGE_CONVERTER_MULTI_THREAD_STACK_H
#define IMAGE_CONVERTER_MULTI_THREAD_STACK_H

#include <stdio.h>
#include "../bitmap.h"

int isempty();

int isfull();

Pixel peek(Pixel* stack);

Pixel pop(Pixel* stack);

extern void push(Pixel data, Pixel* stack);

#endif //IMAGE_CONVERTER_MULTI_THREAD_STACK_H
