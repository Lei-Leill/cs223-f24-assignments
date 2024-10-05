/*----------------------------------------------
 * Author: Lei Lei
 * Date: 10/04/24
 * Description: Read in a PPM file to binary
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_ppm.h"

// Choose *one* to implement (do not remove the other one!)

struct ppm_pixel* read_ppm(const char* filename, int* w, int* h) {
  FILE* file = fopen(filename, "rb");
  if (file == NULL) {
    printf("Error: Read File Failed");
    return NULL;
  }

  // skip header and unnecessary info
  char line[1024] = "";
  fgets(line, sizeof(line), file);
  fgets(line, sizeof(line), file);
  
  fscanf(file, " %d %d", w, h); // read the width and height
  fgets(line, sizeof(line), file); // skip line of the max brightness
  
  // Allocate the array of ppm_pixel in heap
  struct ppm_pixel* result = malloc(sizeof(struct ppm_pixel*) * (*w) * (*h));
  if (result == NULL) { // memory allocation failed
    return NULL;
  }

  fread(result, sizeof(struct ppm_pixel), (*w) * (*h), file); // read the pixel data to binary 
  
  fclose(file);
  return result;
}

struct ppm_pixel** read_ppm_2d(const char* filename, int* w, int* h) {
  return NULL;
}
