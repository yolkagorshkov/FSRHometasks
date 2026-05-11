#include "structures.h"

void swapC(char* a, char* b) {
    if (a == b) return;
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

void swapB(void* a, void* b, int bytes) {
    if (a == b || !a || !b) return;
    for (int i = 0; i < bytes; i++) swapC((char*)a + i, (char*)b + i);
}

void putB(void* to, void* from, int bytes) {
    if (to == from || !to || !from) return;
    for (int i = 0; i < bytes; i++) *((char*)to + i) = *((char*)from + i);
}

// VECTOR

void initVector(vector* v, int _elSize) {
    v->array = NULL;
    v->capacity = v->size = 0;
    v->elSize = _elSize;
}

void clearVector(vector* v) {v->size = 0;}

void freeVector(vector* v) {
    v->capacity = v->size = 0;
    free(v->array);
    v->array = NULL;
}

void allocateVector(vector* v, int n) {
    v->capacity = v->size = n;
    v->array = realloc(v->array, v->capacity * v->elSize);
}

void resizeVector(vector* v) {
    v->capacity = ((v->capacity + 1) * 3) >> 1;
    v->array = realloc(v->array, v->capacity * v->elSize);
}

void* getElement(vector* v, int ind) {
    return (void*)((char*)v->array + ind * v->elSize);
}

void pushBack(vector* v, void* elptr) {
    if (v->size == v->capacity) resizeVector(v);
    void* address = (char*)v->array + v->size * v->elSize;
    putB(address, elptr, v->elSize);
    ++v->size;
}

void popBack(vector* v, void* to) {
    if (!v->size) return;
    --v->size;
    if (to) {
        void* address = (char*)v->array + (v->size * v->elSize);
        putB(to, address, v->elSize);
    }
    if (v->capacity > 4 && v->size * 3 < v->capacity) {
        v->capacity = (v->capacity >> 1) + 1;
        v->array = realloc(v->array, v->capacity * v->elSize);
    }
}

// END OF VECTOR

void initQueue(queue* q) {
    initVector(&q->v, sizeof(int));
    q->head = 0;
}

void freeQueue(queue* q) {
    freeVector(&q->v);
    q->head = 0;
}

void queuePushBack(queue* q, int p) {
    pushBack(&q->v, &p);
}

int queueSize(queue* q) {
    return q->v.size - q->head;
}

int queuePopFront(queue* q) {
    int val = *(int*)getElement(&q->v, q->head);
    ++q->head;
    return val;
}