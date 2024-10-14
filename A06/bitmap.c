/*----------------------------------------------
 * Author: Lei Lei
 * Date: 10/10/2024
 * Description: Reads in a single 64-bit unsigned integer (e.g. unsigned long) and outputs it as an 8x8 1bpp sprite
 ---------------------------------------------*/
#include <stdio.h>

int main() {
  unsigned long img;
  scanf(" %lx", &img);
  printf("Image (unsigned long): %lx\n", img);

  for (int i = 0; i < 8; i ++){
    for (int j = 0; j < 8; j ++){
      int pos = 63 - i * 8 - j;
      if (img & 0x1ul << pos){
        printf("@ ");
      }
      else { printf("  "); }
    }
    printf("\n");
  }
  return 0;
}
