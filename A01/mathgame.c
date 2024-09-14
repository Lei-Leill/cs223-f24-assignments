/***************************************************
 * mathgame.c
 * Author: Lei Lei
 * Implements a math game
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
  printf("Welcome to Math Game!\n");
  printf("How many rounds do you want to play? ");
  int num_q;
  scanf("%d",&num_q);
  
  int num_correct = 0;
  for(int i = 0; i < num_q; i++){
    int rand_num1 = (rand() % 9) + 1;
    int rand_num2 = (rand() % 9) + 1;
    int correct_ans = rand_num1 + rand_num2;
    printf("%d + %d = ? ", rand_num1, rand_num2);
	
    int user_ans;
    scanf(" %d", &user_ans);
	
    if(correct_ans == user_ans){
      num_correct += 1;
      printf("Correct!\n");
    } 
    else{
      printf("Incorrect :(\n");}
    }

  printf("You answered %d/%d correctly\n",num_correct, num_q);
  
  return 0;
}