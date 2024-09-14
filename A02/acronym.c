/*
  File: acronym.c
  Author: Lei Lei
  Description: This program changes a phrase (capital letter of words) to acronym
 */

#include <stdio.h>
#include <string.h>

int main() {
 printf("Enter a phrase: ");
 char phrase[1024];
 scanf("%[^\n]%*c", phrase);
  
 char acronym[512];
 int index = 0;
 for (int i = 0; i < strlen(phrase); i ++){
   if (i == 0 || phrase[i-1] == ' '){
      char c = phrase[i];
      if (c >= 65 && c <= 90){
        acronym[index] = c;
        index += 1;
      }
   }
 }

 printf("Your acroynm is %s\n",acronym);

 return 0;
}
