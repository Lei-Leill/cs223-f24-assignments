/*
  File: exclaim.c
  Author: Lei Lei
  Description: This program changes the small case char in string to one of @!#* randomly
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char buff[32];
  printf("Enter a word: ");
  scanf("%s" , buff);

  char lst[5] = "@!#*";
  char output[32];
  for (int i = 0; i < strlen(buff); i++){
    char c = buff[i];
    if (c >= 97 && c <= 122){
      int rand_ind = rand() % 4;
      output[i] = lst[rand_ind];
    }
    else{
      output[i] = c;
    }

  }

  printf("OMG! %s \n", output); 
  return 0;
}
