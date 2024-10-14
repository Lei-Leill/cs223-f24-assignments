/*----------------------------------------------
 * Author: Lei Lei
 * Date: 10/12/2024
 * Description: Read the ppm file and encode the message inside
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "read_ppm.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("usage: decode <file.ppm>\n");
    return 0;
  }
  
  int w, h;
  struct ppm_pixel* pixels = read_ppm(argv[1], &w, &h);
  if (pixels == NULL){
    return 0;
  }

  int length = w * h * 3;
  int* message= malloc(sizeof(int) * length);
  if (!message) {
    printf("Memory allocation failed");
    free(pixels);
    return 0;
  }

  printf("Reading %s with width %d and height %d\n", argv[1], w, h);
  printf("Max number of characters in the image: %d\n", length/8);
  int index = 0;

  for (int i = 0; i < w * h; i ++){
    message[index] = (pixels[i].red & 0x01);
    index ++;
    message[index] = (pixels[i].green & 0x01);
    index ++;
    message[index] = (pixels[i].blue & 0x01);
    index ++;
  }

  char* output= malloc(sizeof(char) * (length/8 + 1));
  if (!output) {
    printf("Memory allocation failed");
    free(message);
    free(pixels);
    return 0;
  }
  int pos = 0;
  int dec = 0;
  for (int j = 0; j < length; j ++){
    dec = dec << 1; // move 1 rightward to have space for the next bit
    dec = dec | message[j]; // bitwise OR to add the bit to dec
    if ((j+1) % 8 == 0){
      if (dec == 0){
        break; // Stop decoding if we reach '\0'
      }
      char current_char = (char)dec;
      output[pos] = current_char;
      dec = 0;
      pos += 1;
    }
  }
  output[pos] = '\0';
  printf("%s\n", output);
  
  free(output);
  free(message);
  free(pixels);
  
  return 0;
}

