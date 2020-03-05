#ifndef STACK_H
#define	STACK_H

#include <stdlib.h> 
#include <string.h>

// A structure to represent a stack 
typedef struct StackTag {
    int top;
    int capacity;
    void* array;
    int memberSize;
} Stack;

// function to create a stack of given capacity. It initializes size of stack as 0 
Stack* createStack(int memberSize, int capacity);

int destroyStack(Stack* stack);

// Stack is full when top is equal to the last index 
int isFull(Stack* stack);

// Stack is empty when top is equal to -1 
int isEmpty(Stack* stack);

//doubles total capacity of the stack
void expandStack(Stack* stack);

// Function to add an item to stack. It increases top by 1 
int push(Stack* stack, void* item);

// Function to remove an item from stack. It decreases top by 1 
int pop(Stack* stack, void* target);

// Function to return the top from stack without removing it 
int peek(Stack* stack, void* target);

#endif	/* STACK_H */
