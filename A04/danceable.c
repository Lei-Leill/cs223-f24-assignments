/*----------------------------------------------
 * Author: Lei Lei
 * Date: Sept 26
 * Description: Store song info in a csv file into a linked list
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct linkedSong{
  char name[128];
  char artist[128];
  int duration;
  float dance;
  float energy;
  float tempo;
  float valence;
  struct linkedSong* next; 
} linkedSong;


linkedSong* insert_list(char line[512], linkedSong* prev){
  // this function return a linkedSong object based on given line
  linkedSong* current = malloc(sizeof(linkedSong));

  char *token = strtok(line, ",");
  strcpy(current->name, token);

  token = strtok(NULL, ","); // Null helps strtok to continue from the last position it remembers
  strcpy(current->artist, token);

  token = strtok(NULL, ",");
  current->duration = atoi(token);

  token = strtok(NULL, ",");
  current->dance = atof(token);

  token = strtok(NULL, ",");
  current->energy = atof(token);

  token = strtok(NULL, ",");
  current->tempo = atof(token);

  token = strtok(NULL, ",");
  current->valence = atof(token);

  current->next = prev;
  return current;
}

void print_list(linkedSong* song){
  int i = 0;
  while (song){
    printf("%d) %-25s artist: %-20s ", i, song->name, song->artist);
    int minute = song->duration / 1000 / 60;  // the division '/' can ensure it is an integer
    int second = song->duration / 1000 - minute * 60; 
    printf("(%d:%02ds)", minute, second);
    printf(" D: %-6.3f E: %-6.3f T: %-6.3f V: %-6.3f\n",
           song->dance, song->energy, song->tempo, song->valence);

    song = song->next;
    i ++;
  }
}

void find_remove_most_dance(linkedSong** head){
  if (*head == NULL) { // prevent segmantation error
    return;  // Exit the function
  }

  linkedSong* most_d = *head;        // Start with the head as the most danceable
  linkedSong* prev_most_d = NULL;    // To track the previous node of the most danceable
  linkedSong* prev = NULL;            // To keep track of the previous node during traversal
  linkedSong* song = *head;           // Using head directly for traversal

  // Traverse the list to find the most danceable song
  while (song != NULL) {
    if (song->dance > most_d->dance) {
      most_d = song;                
      prev_most_d = prev;          
    }
    prev = song;                   // Move prev to the current song
    song = song->next;             // Move to the next song
  }

  
  if (prev_most_d == NULL) {      // If the most danceable song is the head
    *head = most_d->next;
  } else {
    prev_most_d->next = most_d->next; // Bypass the most danceable song
  }

  printf("---------------------------------------- Most danceable ------------------------------------\n");
  print_list(most_d);
  printf("--------------------------------------------------------------------------------------------\n");

  // Free the most danceable song
  free(most_d);

  print_list(*head);
}

void free_all(linkedSong* song){
  while(song){
    linkedSong* temp = song->next;
    free(song);
    song = temp;
  }
}
int main() {
  FILE *file = fopen("songlist.csv", "r");
  if (file == NULL) {
    printf("Unable to open file \n\n");
    return 1;
  }
  
  char line[512]; // line variable used to temporarily hold info
  fgets(line, sizeof(line), file);   // Skip the header line
  
  linkedSong* current = NULL;
  linkedSong* prev = NULL;
  prev = NULL;
  int num_songs = 0;

  while(fgets(line, sizeof(line), file)){
    current = insert_list(line, prev);
    prev = current;
    num_songs ++;
  }
  fclose(file);  

  print_list(current);
  printf("\nDataset contains %d songs\n", num_songs);
  printf("===========================\n");
  char input;
  printf("Press 'd' to show the most danceable song (any other key to quit): ");
  scanf(" %c", &input);

  while (input == 'd'){
    find_remove_most_dance(&current); //has to be pointer of pointer since we might change the head
    num_songs --;
    if (num_songs < 0) {num_songs = 0;} // prevent negative num
    printf("\nDataset contains %d songs\n", num_songs);
    printf("===========================\n");
    printf("Press 'd' to show the most danceable song (any other key to quit): ");
    scanf(" %c", &input);
  }
  
  free_all(current);
  return 0;
}

