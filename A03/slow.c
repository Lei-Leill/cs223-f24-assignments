/*----------------------------------------------
 * Author: Lei Lei
 * Date: 09/20/2024
 * Description: This program turns a phrase into what Ents will speak
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  printf("Pause Length: ");
  int length;
  scanf("%d", &length);

  printf("Text: ");
  char text[32];
  scanf("%s", text);

  char* output_text;
  int output_len = strlen(text) * (length + 1) ; // the length = size (of input) + size * pause length
  output_text = malloc(sizeof(char) * (output_len + 1));

  int j = 0; // j to be the index of output text
  for(int i = 0; i < strlen(text); i ++){
    output_text[j] = text[i];
    j += 1;
    for(int k = 0; k < length; k ++){
      output_text[j] = '.';
      j += 1;
    }
  }

  printf("%s \n", output_text);
  free(output_text);
  return 0;
}
