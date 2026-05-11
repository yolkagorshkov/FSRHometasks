#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdlib.h>

void swapC(char*, char*);
void swapB(void*, void*, int);
void putB(void*, void*, int);

typedef struct vector{
    int size, capacity, elSize;
    void* array;
} vector;

void initVector(vector*, int);
void clearVector(vector*);
void freeVector(vector*);
void resizeVector(vector*);
void allocateVector(vector*, int);
void* getElement(vector*, int);
void pushBack(vector*, void*);
void popBack(vector*, void*);


typedef struct queue{
    vector v;
    int head, tail;
} queue;

void initQueue(queue*);
void freeQueue(queue*);
void queuePushBack(queue*, int);
int queueSize(queue*);
int queuePopFront(queue*);

#endif