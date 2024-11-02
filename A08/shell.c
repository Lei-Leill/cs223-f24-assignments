/*----------------------------------------------
 * Author: Lei Lei
 * Date: 10/30/24
 * Description: Create my own shell
 ---------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>


#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_GREEN  "\x1b[32m"
#define ANSI_COLOR_RESET  "\x1b[0m"

int main() {
  printf("Welcome to the Smile Shell :):):)\n");
  printf("-----------------------------------\n");
  printf("-----------------------------------\n");
  char* input = malloc(sizeof(char) * 100);

  char hostname[512];
  gethostname(hostname, sizeof(hostname) - 1);

  struct passwd *pw = getpwuid(geteuid());
  char *username = pw->pw_name;

  while(1){
    char prompt[2048];
    char directory[512];
    getcwd(directory, 511);
    
    snprintf(prompt, sizeof(prompt), 
                 ANSI_COLOR_YELLOW "%s@%s" ANSI_COLOR_RESET 
                 ":" ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET " :):):)  ", 
                 username, hostname, directory);

    input = readline(prompt);
    if (*input && input){
      add_history(input);
    }
    if (strcmp(input, "exit") == 0){
      free(input);
      return 0;
    }

    char* args[100];
    int index = 0;
    char *token = strtok(input, " ");
    while (token != NULL){
      args[index] = token;
      token = strtok(NULL, " ");
      index += 1;
    }
    args[index] = NULL;

    pid_t pid;
    pid = fork();
    int status;
    if (pid < 0){
      printf("Fork Failed");
      return 1;
    }
    else if (pid == 0){
      if (execvp(args[0], args) == -1) { // execute the command and check if it fails
          perror("Error executing command");
          exit(EXIT_FAILURE); // Exit child process if execvp fails
      }
      exit(0);
    }
    else{
      waitpid(pid, &status, 0);
    }

  }
  free(input);
  return 0;
}
