/*----------------------------------------------
 * Author: Lei Lei
 * Date: 10/30/24
 * Description: Fork processes and print the result out
 ---------------------------------------------*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


int main() {
  pid_t pid;
  printf("%d] A0 \n", getpid());
  pid = fork();
  if (pid < 0){
    printf("Fork Failed");
    return 1;
  }
  else if (pid){   //!= 0 -> parent case
    printf("%d] B0\n", getpid());
    pid = fork();
    if (pid < 0){
      printf("Fork Failed");
      return 1;
    }
    else if(pid){
      printf("%d] C0\n", getpid());
    }
    else{
      printf("%d] C1\n", getpid());
    }
  }
  else{ 
    printf("%d] B1\n", getpid());
  }
  
  printf("%d] Bye\n", getpid());

  return 0;
}
