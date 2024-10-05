/*----------------------------------------------
 * Author: Lei Lei
 * Date: 10/03/24
 * Description: Generate crossword based on the words given by user
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("Usage: %s <word1> <word2>\n", argv[0]);
    exit(0);
  }
  
  int index_1 = -1;
  int index_2 = -1;
  for(int i = 0; i < strlen(argv[1]); i ++){
    char c = argv[1][i];
    for(int j = 0; j < strlen(argv[2]); j ++){
      if (c == argv[2][j]){
        index_1 = i;
        index_2 = j;
        break;
      }
    }
    if(index_1 != -1){ 
      break;  //break if we found a common letter
    }
  }

  if (index_1 == -1){
    printf("No common letter!\n");
    return 0;
  }

  // Creating the 2D array in heap
  char** array = malloc(sizeof(char *) * strlen(argv[1]));
  for (int i = 0; i < strlen(argv[1]); i ++){
    array[i] = malloc(sizeof(char) * strlen(argv[2]));
    for(int j = 0; j < strlen(argv[2]); j ++){
      array[i][j] = '.';
    }
  }

  // Put the two strings into the 2D array
  for(int i = 0; i < strlen(argv[2]); i ++){
    array[index_1][i] = argv[2][i]; // put character in second string to the row
  }

  for(int i = 0; i < strlen(argv[1]); i ++){
    array[i][index_2] = argv[1][i]; // put character in first string to the row
  }

  // Traverse the 2D array to print it out
  for (int i = 0; i < strlen(argv[1]); i ++){
    for(int j = 0; j < strlen(argv[2]); j ++){
      printf("%c ", array[i][j]);
    }
    printf("\n");
  } 

  for (int i = 0; i < strlen(argv[1]); i ++){
    free(array[i]);
  } 
  free(array);

  return 0;
}
