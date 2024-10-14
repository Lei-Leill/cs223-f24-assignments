/*----------------------------------------------
 * Author: 
 * Date: 
 * Description
 ---------------------------------------------*/
#include "write_ppm.h"
#include <stdio.h>
#include <string.h>

// implement *one* (but do not remove the other one)!

void write_ppm(const char* filename, struct ppm_pixel* pixels, int w, int h) {
    FILE* file = fopen(filename, "wb");
    fprintf(file, "P6\n# Created by write_ppm method\n");
    fprintf(file, "%i %i \n255\n", w, h);
    fwrite(pixels, sizeof(struct ppm_pixel), w * h, file);
    fclose(file);
}

void write_ppm_2d(const char* filename, struct ppm_pixel** pixels, int w, int h) {

}
