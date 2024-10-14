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
  int max_chars = w * h * 3 / 8;
  printf("Max number of characters in the image: %d\n", max_chars);
  
  char* message = malloc(sizeof(char) * (max_chars + 1));
  printf("Enter a phrase: ");
  scanf("%s", message);

  if (strlen(message) > max_chars) {
    printf("Error: Message exceeds maximum length of %d characters.\n", max_chars);
    free(pixels);
    return 1;
  }

  int num_chars = strlen(message) + 1;
  unsigned char *binary_message = malloc(num_chars * 8 * sizeof(unsigned char));

  // Convert the input message to binary representation
  for (int i = 0; i < num_chars; i++) { // traverse each character
    for (int bit_index = 0; bit_index < 8; bit_index++) {
      int pos = i * 8 + bit_index;
      int shifted_bit = message[i] >> (7 - bit_index); // Right shift the ASCII value of message[i]
      binary_message[pos] = shifted_bit & 1; // Extract the least significant bit and store it
    }
  }

  // Encode the binary data into the pixel channels
  for (int i = 0; i < num_chars * 8; i++) {
    int pixel_index = i / 3;
    unsigned char *curr_color = NULL;

    // Determine which color channel to modify
    if (i % 3 == 0) {
        curr_color = & pixels[pixel_index].red; //pointing to red
    } else if (i % 3 == 1) {
        curr_color = & pixels[pixel_index].green; //pointing to green
    } else {
        curr_color = & pixels[pixel_index].blue; //pointing to blue
    }

    // Modify the color channel when mismatch
    if ((*curr_color & 1) != binary_message[i]) {
      if ((*curr_color + 1) > 255){
        *curr_color -= 1;
      }
      else{
        *curr_color += 1;
      }
    }
  }

  // Construct the new filename
  size_t old_len = strlen(argv[1]);
  char *new_name = malloc((old_len + 9) * sizeof(char));
  strncpy(new_name, argv[1], old_len - 4);
  new_name[old_len - 4] = '\0'; // Null terminate
  strcat(new_name, "-encoded.ppm");

  printf("Writing file %s\n", new_name);
  write_ppm(new_name, pixels, w, h);

  // Free allocated memory
  free(binary_message);
  free(new_name);
  free(message);
  free(pixels);
  return 0;
}

