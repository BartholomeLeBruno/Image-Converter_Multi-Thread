//
// Created by alexis on 09/01/20.
//
#include <stdio.h>
#include "../bitmap.h"
#include "stack.h"

int MAXSIZE = __INT_MAX__;
int top = -1;

int isempty() {

    if(top == -1)
        return 1;
    else
        return 0;
}

int isfull() {

    if(top == MAXSIZE)
        return 1;
    else
        return 0;
}

Pixel peek(Pixel* stack) {
    return stack[top];
}

Pixel pop(Pixel* stack) {
    Pixel data;

    if(!isempty()) {
        data = stack[top];
        top = top - 1;
        return data;
    } else {
        printf("Could not retrieve data, Stack is empty.\n");
    }
}

void push(Pixel data, Pixel* stack) {

    if(!isfull()) {
        top = top + 1;
        stack[top] = data;
    } else {
        printf("Could not insert data, Stack is full.\n");
    }
}