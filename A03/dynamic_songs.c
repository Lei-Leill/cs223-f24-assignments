/*----------------------------------------------
 * Author: Lei Lei
 * Date: 09/20/2024
 * Description: This program reads songs from a text file into an array
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Song{
  char name[128];
  char artist[128];
  int duration;
  float dance;
  float energy;
  float tempo;
  float valence;
} Song;

void print_songs(Song songs[], int num_songs){
  for(int i = 0; i < num_songs; i ++){
    printf("%d) %-25s artist: %-20s ", i, songs[i].name, songs[i].artist);
    int minute = songs[i].duration / 1000 / 60;  // the division '/' can ensure it is an integer
    int second = songs[i].duration / 1000 - minute * 60; 
    printf("duration: %d:%02d", minute, second);
    printf(" D: %-6.3f E: %-6.3f T: %-6.3f V: %-6.3f\n",
           songs[i].dance, songs[i].energy, songs[i].tempo, songs[i].valence);
  }
}

int main() {
  printf("Welcome to Dynamic Donna's Danceability Directory.\n");

  FILE *file = fopen("songlist.csv", "r");
  if (file == NULL) {
    printf("Unable to open file \n\n");
    return 1;
  }
  
  char line[512]; // line variable used to temporarily hold info
  fgets(line, sizeof(line), file);
  int num_songs = atoi(strtok(line, ",")); // Get the first token for number of songs
  fgets(line, sizeof(line), file);   // Skip the second line
  Song* songs = malloc(sizeof(Song) * num_songs); // Creat the Song Array in heap

  for(int i = 0; i < num_songs; i++){
    fgets(line, sizeof(line), file);
    char *token = strtok(line, ",");
    strcpy(songs[i].name, token);

    token = strtok(NULL, ","); // Null helps strtok to continue from the last position it remembers
    strcpy(songs[i].artist, token);

    token = strtok(NULL, ",");
    songs[i].duration = atoi(token);

    token = strtok(NULL, ",");
    songs[i].dance = atof(token);

    token = strtok(NULL, ",");
    songs[i].energy = atof(token);

    token = strtok(NULL, ",");
    songs[i].tempo = atof(token);

    token = strtok(NULL, ",");
    songs[i].valence = atof(token);
  }
  
  print_songs(songs, num_songs);  
  free(songs);
  return 0;
}
