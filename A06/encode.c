/*----------------------------------------------
 * Author: Lei Lei
 * Date: 10/13/2024
 * Description: Read in a PPM and embed a message from user to embed in
 ---------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_ppm.h"
#include "write_ppm.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("usage: encode <file.ppm>\n");
    return 0;
  }

  int w, h;
  struct ppm_pixel* pixels = read_ppm(argv[1], &w, &h);
  if(pixels == NULL){
    return 1;
  }
  printf("Reading %s with width %d and height %d\n", argv[1], w, h);
  int length = w * h * 3;
  printf("Max number of characters in the image: %d\n", length/8);
  
  char* message = malloc(sizeof(char) * length/8);
  printf("Enter a phrase: ");
  scanf("%s", message);

  free(message);
  return 0;
}

