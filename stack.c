/*
 * File:   stack.c
 * Author: Sergey
 *
 * Created on November 25, 2019, 2:46 PM
 */

// C program for array implementation of stack 
#include "stack.h"

Stack* createStack(int memberSize, int capacity)
{
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = malloc(capacity * memberSize);
    stack->memberSize = memberSize;
    return stack;
}

int destroyStack(Stack* stack) {
    free(stack->array);
    free(stack);
    return 0;
}

int isFull(Stack* stack)
{
    return stack->top == stack->capacity - 1;
}

int isEmpty(Stack* stack)
{
    return stack->top == -1;
}

void expandStack(Stack* stack) {
    stack->array = realloc(stack->array, stack->capacity * 2);
    stack->capacity *= 2;
}

int push(Stack* stack, void* item)
{
    if (isFull(stack)) {
        // alternative: expandStack(stack);
        return 1;
    }
    stack->top++;
    void* target = (char*)stack->array + (stack->top * stack->memberSize);
    memcpy(target, item, stack->memberSize);
    return 0;
}

int pop(Stack* stack, void* target)
{
    if (isEmpty(stack)) {
        return 1;
    }
    void* source = (char*)stack->array + (stack->top * stack->memberSize);
    stack->top--;
    if (target)
        memcpy(target, source, stack->memberSize);
    return 0;
}

int peek(Stack* stack, void* target)
{
    if (isEmpty(stack)) {
        return 1;
    }
    void* source = (char*)stack->array + (stack->top * stack->memberSize);
    memcpy(target, source, stack->memberSize);
    return 0;
}