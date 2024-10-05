/*----------------------------------------------
 * Author: Lei Lei
 * Date: 10/04/24
 * Description: Test whether we can write binary to ppm
 ---------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_ppm.h"
#include "write_ppm.h"

int main(int argc, char** argv) {
  int w, h;
  struct ppm_pixel* pixels = read_ppm("feep-raw.ppm", &w, &h);

  // test writing the file to test.ppm, reload it, and print the contents
  write_ppm("test.ppm", pixels, 4, 4);
  printf("Testing file %s: %i %i\n", "free-raw.ppm",  w, h);
  struct ppm_pixel* test_pixels = read_ppm("test.ppm", &w, &h);

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      printf("(%i,%i,%i) ", test_pixels[i * w + j].red, test_pixels[i * w + j].green, test_pixels[i * w + j].blue);
    }
    printf("\n");
  }
  free(pixels);
  return 0;
}
