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

struct Img peek(struct Img* stack) {
    return stack[top];
}

struct Img* pop(struct Img** stack) {
    struct Img* data;

    if(!isempty()) {
        *data = *stack[top];
        top = top - 1;
        return data;
    } else {
        printf("Could not retrieve data, Stack is empty.\n");
    }
}

void push(struct Img* data, struct Img** stack) {
    printf("sizeof : %d\n", sizeof(stack));
    if(!isfull()) {
        top = top + 1;
        *stack[top] = *data;
    } else {
        printf("Could not insert data, Stack is full.\n");
    }
}