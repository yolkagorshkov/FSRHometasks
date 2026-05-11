#ifndef IMAGEWORK_H
#define IMAGEWORK_H

#define REDMASK (255 << 0)
#define GREENMASK (255 << 8)
#define BLUEMASK (255 << 16)
#define ALPHAMASK (255 << 24)

#define RED(val) (unsigned char)((val & REDMASK) >> 0)
#define GREEN(val) (unsigned char)((val & GREENMASK) >> 8)
#define BLUE(val) (unsigned char)((val & BLUEMASK) >> 16)
#define ALHPA(val) (unsigned char)((val & ALPHAMASK) >> 24)



typedef struct Image{
    unsigned int w, h;
    unsigned char* image;
} Image;

Image* imageAllocate();
const char* errorText(int error);
int loadPng(const char* filename, Image* image);
int writePng(const char* filename, Image* image);
int* getPixelValue(Image* image, int x, int y);
void freeImage(Image** image);
void imageVerticalLineCut(Image* image, int x, char s);
void imageEraseRectangle(Image* image, int x1, int y1, int x2, int y2);
void imageDrawLine(Image* image, int p, char d);
void imageDrawCross(Image* image, int x0, int y0);
void imageConvolutionTotal(double** core, int n, Image* image, Image* newImage);
void imageSobelTotal(Image* image, Image* newImage);
void imageGaussTotal(int r, Image* image, Image* newImage);
void imageContrastTotal(Image* image, Image* newImage);
void imageHardContrastTotal(Image* image, Image* newImage, int c);
void BFSFiltering(Image* image);
void masking(Image* image, Image* mask);
int BFSctr(Image* image);

#endif