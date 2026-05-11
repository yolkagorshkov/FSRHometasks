#include "../lodepng/lodepng.h"
#include "imageWork.h"
#include "../usefulMath/usefulMath.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../structures/structures.h"

#define MIN(x, y) (x < y) ? x : y
#define MAX(x, y) (x > y) ? x : y

const double sobelX[][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
const double sobelY[][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};

int copySobel(char d, double*** mtx) {
    *mtx = (double**)malloc(3 * sizeof(double*));
    if (!mtx) return 1;
    if (d == 'x') {
        for (int i = 0; i < 3; i++) {
            (*mtx)[i] = (double*)malloc(3 * sizeof(double));
            (*mtx)[i][0] = sobelX[i][0];
            (*mtx)[i][1] = sobelX[i][1];
            (*mtx)[i][2] = sobelX[i][2];
        }
    }
    else {
        for (int i = 0; i < 3; i++) {
            (*mtx)[i] = (double*)malloc(3 * sizeof(double));
            (*mtx)[i][0] = sobelY[i][0];
            (*mtx)[i][1] = sobelY[i][1];
            (*mtx)[i][2] = sobelY[i][2];
        }
    }
    return 0;
}

Image* imageAllocate() {
    return (Image*)calloc(sizeof(Image), 1);
}

const char* errorText(int error) {
    return lodepng_error_text(error);
}

int loadPng(const char* filename, Image* image) {
    int err = lodepng_decode32_file(&(image->image), &(image->w), &(image->h), filename);
    return err;
}

int writePng(const char* filename, Image* image) {
    unsigned char* png;
    unsigned long long pngSize;
    int err = lodepng_encode32(&png, &pngSize, image->image, image->w, image->h);
    if (err != 0) return err;
    err = lodepng_save_file(png, pngSize, filename);
    return err;
}

int* getPixelValue(Image* image, int x, int y) {
    if (0 <= y && y < image->h && 0 <= x && x < image->w) {
        return &((int*)(image->image))[y * (image->w) + x];
    }
    return 0;
}

void imageVerticalLineCut(Image* image, int x1, char s) {
    if (s == 'l') {
        for (int y = 0; y < image->h; y++) {
            for (int x = 0; x < x1; x++) *getPixelValue(image, x, y) = ALPHAMASK + 0;
        }
    }
    else {
        for (int y = 0; y < image->h; y++) {
            for (int x = x1; x < image->w; x++) *getPixelValue(image, x, y) = ALPHAMASK + 0;
        }
    }
}

void imageDrawLine(Image* image, int p, char d) {
    if (d == 'x') {
        for (int y = 0; y < image->h; y++) *getPixelValue(image, p, y) = ALPHAMASK + GREENMASK;
    }
    else for (int x = 0; x < image->w; x++) *getPixelValue(image, x, p) = ALPHAMASK + GREENMASK;
}
void imageDrawCross(Image* image, int x0, int y0) {
    for (int x = x0 - 3; x <= x0 + 3; x++) *getPixelValue(image, x, y0) = ALPHAMASK + GREENMASK;
    for (int y = y0 - 3; y <= y0 + 3; y++) *getPixelValue(image, x0, y) = ALPHAMASK + GREENMASK;
}

void imageEraseRectangle(Image* image, int x1, int y1, int x2, int y2) {
    for (int x = x1; x < x2; x++) {
        for (int y = y1; y < y2; y++) {
            *getPixelValue(image, x, y) = ALPHAMASK + 0;
        }
    }
}

int checkAdd(queue* q, int x, int y, Image* image, char *used) {
    int v = y * image->w + x;
    if (!used[v] && RED(*getPixelValue(image, x, y))) {
        used[v] = 1;
        queuePushBack(q, v);
        return 1;
    }
    return 0;
}   

void BFSFiltering(Image* image) {
    queue q; initQueue(&q);
    char* used; used = (char*)calloc(image->h * image->w, sizeof(char));
    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < image->w; x++) {
            if (!used[y * image->w + x] && RED(*getPixelValue(image, x, y))) {
                used[y * image->w + x] = 1;
                queuePushBack(&q, y * image->w + x);
                vector visited; initVector(&visited, sizeof(int));
                while (queueSize(&q)){
                    int u = queuePopFront(&q);
                    pushBack(&visited, &u);
                    int yu = u / image->w, xu = u % image->w;
                    if (xu > 0) checkAdd(&q, xu-1, yu, image, used);
                    if (xu < image->w - 1) checkAdd(&q, xu+1, yu, image, used);
                    if (yu > 0) checkAdd(&q, xu, yu-1, image, used);
                    if (yu < image->h - 1) checkAdd(&q, xu, yu+1, image, used);
                }
                if (visited.size < 1000) {
                    for (int i = 0; i < visited.size; i++) {
                        int u = *(int*)getElement(&visited, i);
                        int y = u / image->w, x = u % image->w;
                        *getPixelValue(image, x, y) = 0 + ALPHAMASK;
                    }
                }
                freeVector(&visited);
            }
        }
    }
    freeQueue(&q);
    free(used);
}

void masking(Image* image, Image* mask) {
    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < image->w; x++) {
            if (RED(*getPixelValue(mask, x, y))) {
                *getPixelValue(image, x, y) = ALPHAMASK;
            }
        }
    }
}

int BFSctr(Image* image) {
    int ctr = 0;
    queue q; initQueue(&q);
    char* used; used = (char*)calloc(image->h * image->w, sizeof(char));
    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < image->w; x++) {
            if (!used[y * image->w + x] && RED(*getPixelValue(image, x, y))) {
                used[y * image->w + x] = 1;
                queuePushBack(&q, y * image->w + x);
                vector visited; initVector(&visited, sizeof(int));
                int xmn = 1e9;
                int xmx = -1e9;
                int ymn = 1e9;
                int ymx = -1e9;
                while (queueSize(&q)){
                    int u = queuePopFront(&q);
                    pushBack(&visited, &u);
                    int yu = u / image->w, xu = u % image->w;
                    if (xu > 0) {
                        if (checkAdd(&q, xu-1, yu, image, used)) {
                            xmn = MIN(xmn, xu-1);
                            xmx = MAX(xmx, xu-1);
                            ymn = MIN(xmn, yu);
                            ymx = MAX(xmx, yu);
                        }
                    }
                    if (xu < image->w - 1) {
                        if (checkAdd(&q, xu+1, yu, image, used)) {
                            xmn = MIN(xmn, xu+1);
                            xmx = MAX(xmx, xu+1);
                            ymn = MIN(xmn, yu);
                            ymx = MAX(xmx, yu);
                        }
                    }
                    if (yu > 0) {
                        if (checkAdd(&q, xu, yu-1, image, used)) {
                            xmn = MIN(xmn, xu);
                            xmx = MAX(xmx, xu);
                            ymn = MIN(xmn, yu-1);
                            ymx = MAX(xmx, yu-1);
                        }
                    }
                    if (yu < image->h - 1) {
                        if (checkAdd(&q, xu, yu+1, image, used)) {
                            xmn = MIN(xmn, xu);
                            xmx = MAX(xmx, xu);
                            ymn = MIN(xmn, yu+1);
                            ymx = MAX(xmx, yu+1);
                        }
                    }
                }
                if (ymx - ymn > 1 && xmx - xmn > 1) {
                    double midX = 0, midY = 0;
                    for (int i = 0; i < visited.size; i++) {
                        int u = *(int*)getElement(&visited, i);
                        int y = u / image->w, x = u % image->w;
                        midX += x; midY += y;
                    }
                    int xc = midX / visited.size;
                    int yc = midY / visited.size;
                    imageDrawCross(image, xc, yc);
                    ctr += 1;
                }
                freeVector(&visited);
            }
        }
    }
    freeQueue(&q);
    free(used);
    return ctr;
}

int convolution(double** core, int n, int x, int y, Image* image) {
    double sum = 0;
    for (int dy = -(n >> 1); dy <= (n >> 1); dy++) {
        for (int dx = -(n >> 1); dx <= (n >> 1); dx++) {
            double col = 0;
            int* val = getPixelValue(image, x + dx, y + dy);
            if (val) col = (double)(RED(*val) + GREEN(*val) + BLUE(*val)) / 3.0;
            sum += core[dy + (n >> 1)][dx + (n >> 1)] * col;
        }
    }
    unsigned char grey = sum > 0 ? sum : 0;
    int val = grey * (1 + 65536 + 256) + (255 << 24);
    return val;
}

void imageConvolutionTotal(double** core, int n, Image* image, Image* newImage) {
    newImage->h = image->h; newImage->w = image->w;
    newImage->image = (unsigned char*)malloc(image->h * image->w * sizeof(int));
    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < image->w; x++) {
            *getPixelValue(newImage, x, y) = convolution(core, n, x, y, image);
        }
    }
}

void imageSobelTotal(Image* image, Image* newImage) {
    newImage->h = image->h; newImage->w = image->w;
    newImage->image = (unsigned char*)malloc(image->h * image->w * sizeof(int));
    double** coreX;
    double** coreY;
    int err;

    err = copySobel('x', &coreX);
    if (err) {
        printf("error %d\n", err);
        return;
    }

    err = copySobel('y', &coreY);
    if (err) {
        printf("error %d\n", err);
        return;
    }

    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < image->w; x++) {
            double colX = (double)GREEN(convolution(coreX, 3, x, y, image));
            double colY = (double)GREEN(convolution(coreY, 3, x, y, image));
            double ful = sqrt((sqr(colX) + sqr(colY)) / 2.0);

            *getPixelValue(newImage, x, y) = (unsigned char)ful * (1 + 65536 + 256) + (255 << 24);
        }
    }
    freeMtx(3, (void***)(&coreX));
    freeMtx(3, (void***)(&coreY));
    
}

void imageGaussTotal(int r, Image* image, Image* newImage) {
    int n = ((r << 1) | 1);
    newImage->h = image->h; newImage->w = image->w;
    newImage->image = (unsigned char*)malloc(image->h * image->w * sizeof(int));
    int err;
    double** core;
    err = GaussMatrix(r, 1, 1, &core);
    if (err) {
        printf("error %d\n", err);
        return;
    }
    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < image->w; x++) {
            double g = convolution(core, n, x, y, image);

            *getPixelValue(newImage, x, y) = (unsigned char)g * (1 + 65536 + 256) + (255 << 24);
        }
    }
    

    freeMtx(n, (void***)(&core));
}

void imageContrastTotal(Image* image, Image* newImage) {
    newImage->h = image->h; newImage->w = image->w;
    newImage->image = (unsigned char*)malloc(image->h * image->w * sizeof(int));
    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < image->w; x++) {
            double col = 0;
            int* val = getPixelValue(image, x, y);
            if (val) col = (double)(RED(*val) + GREEN(*val) + BLUE(*val)) / 3.0;
            col = (p(smoothstep3(col / 256))) * 256;
            *getPixelValue(newImage, x, y) = (unsigned char)col * (1 + 65536 + 256) + (255 << 24);
        }
    }
}

void imageHardContrastTotal(Image* image, Image* newImage, int c) {
    newImage->h = image->h; newImage->w = image->w;
    newImage->image = (unsigned char*)malloc(image->h * image->w * sizeof(int));
    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < image->w; x++) {
            double col = 0;
            int* val = getPixelValue(image, x, y);
            if (val) col = (double)(RED(*val) + GREEN(*val) + BLUE(*val)) / 3.0;
            //col = smoothstep3(col / 256) * 256;
            col = (col > c) ? 255 : 0;
            *getPixelValue(newImage, x, y) = (unsigned char)col * (1 + 65536 + 256) + (255 << 24);
        }
    }
}

void freeImage(Image** image) {
    if ((*image)->image) free((*image)->image);
    free(*image);
}