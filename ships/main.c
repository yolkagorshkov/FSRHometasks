#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "./src/imageWork/imageWork.h"
#include "./src/usefulMath/usefulMath.h"




#define GR 5
#define SIGMA3 0.8
#define SIGMA5 0.8
#define LAMBDA3 8
#define LAMBDA5 15
#define X1 450



int main(int argc, char argv[]) {
    int err;
    unsigned char* filepath = "../images/first.png";
    Image *conv5, *sobel5, *image, *gauss, *contrast, *gaussPlus, *gaussPcontrast;
    image = imageAllocate();
    conv5 = imageAllocate();
    sobel5 = imageAllocate();
    gauss = imageAllocate();
    contrast = imageAllocate();
    gaussPlus = imageAllocate();
    gaussPcontrast = imageAllocate();

    double **core3, **core5;
    err = LoGMatrix(1, SIGMA3, LAMBDA3, &core3);
    if (err) {
        printf("LoG error");
        freeMtx(3, (void***)(&core3));
        freeImage(&image);
        freeImage(&conv5);
        freeImage(&sobel5);
        freeImage(&contrast);
        freeImage(&gauss);
        freeImage(&gaussPlus);
        freeImage(&gaussPcontrast);
        return 0;
    }
    err = LoGMatrix(2, SIGMA5, LAMBDA5, &core5);
    if (err) {
        printf("LoG error");
        freeMtx(3, (void***)(&core3));
        freeImage(&image);
        freeImage(&conv5);
        freeImage(&sobel5);
        freeImage(&contrast);
        freeImage(&gauss);
        freeImage(&gaussPlus);
        freeImage(&gaussPcontrast);
        return 0;
    }




    err = loadPng(filepath, image);
    if (err) {
        printf("Image load error: %d %s", err, errorText(err));
        freeMtx(3, (void***)(&core3));
        freeImage(&image);
        freeImage(&conv5);
        freeImage(&sobel5);
        freeImage(&contrast);
        freeImage(&gauss);
        freeImage(&gaussPlus);
        freeImage(&gaussPcontrast);
        return 0;
    }


    
    imageGaussTotal(GR, image, gauss);
    
    imageConvolutionTotal(core5, 5, gauss, conv5);
    imageSobelTotal(conv5, sobel5);

    imageGaussTotal(3, image, gaussPlus);
    imageHardContrastTotal(gaussPlus, gaussPcontrast, 20);


    BFSFiltering(gaussPcontrast);


    masking(sobel5, gaussPcontrast);
    imageContrastTotal(sobel5, gaussPcontrast);


    imageVerticalLineCut(gaussPcontrast, 450, 'l');
    imageVerticalLineCut(gaussPcontrast, 1100, 'r');
    imageEraseRectangle(gaussPcontrast, 700, 0, 1200, 300);

    imageHardContrastTotal(gaussPcontrast, contrast, 40);


    imageEraseRectangle(contrast, 450, 0, 536, 300);
    int ans = BFSctr(contrast);
    printf("%d", ans);

    err = writePng("../images/marked.png", contrast);
    if (err) {
        printf("Image write error: %d %s", err, errorText(err));
        freeMtx(3, (void***)(&core3));
        freeImage(&image);
        freeImage(&conv5);
        freeImage(&sobel5);
        freeImage(&contrast);
        freeImage(&gauss);
        freeImage(&gaussPlus);
        freeImage(&gaussPcontrast);
        return 0;
    }




    freeMtx(3, (void***)(&core3));
    freeImage(&image);
    freeImage(&conv5);
    freeImage(&sobel5);
    freeImage(&contrast);
    freeImage(&gauss);
    freeImage(&gaussPlus);
    freeImage(&gaussPcontrast);
    

    return 0;
}