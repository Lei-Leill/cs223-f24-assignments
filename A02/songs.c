/*
  File: song.c
  Author: Lei Lei
  Description: This program fills an array with three or more songs and then 
                allows the user to edit the data in the array
 */

#include <stdio.h>
#include <string.h>

typedef struct song{
  char title[32];
  char artist[32];
  int minute;
  int second;
  float danceability;
} song;

int print_songs(song songs[]){
  for(int i = 0; i < 3; i ++){
      printf("%d) %-22s artist: %-20s duration: %d:%02d  danceability: %.2f\n",
          i, songs[i].title, songs[i].artist, songs[i].minute, songs[i].second, songs[i].danceability);
  }
  printf("==================\n");
  return 0;
}

int main() {
  printf("Welcome to Steven Struct's Song List. \n");
  song s1, s2, s3;
 
 // Initialize song 1
  strcpy(s1.title, "Shout");
  strcpy(s1.artist, "Tears for Fears");
  s1.minute = 4;
  s1.second = 11;
  s1.danceability = 0.50;

  // Initialize song 2
  strcpy(s2.title, "As it was");
  strcpy(s2.artist, "Harry Styles");
  s2.minute = 2;
  s2.second = 47;
  s2.danceability = 0.70;

  // Initialize song 3
  strcpy(s3.title, "Wish you were here");
  strcpy(s3.artist, "Pink Floyd");
  s3.minute = 5;
  s3.second = 34;
  s3.danceability = 0.30; 
 
  song songs[3];
  songs[0] = s1;
  songs[1] = s2;
  songs[2] = s3;

  print_songs(songs);

  printf("Enter a song id to edit [0,1,2]: ");
  int index;
  scanf("%d" , &index);
  if (index != 0 && index != 1 && index !=2){
    printf("Invalid Choice\n");
    return 0;
  }
 
  printf("\nWhich attribute do you wish to edit? [artist, title, duration, danceability]: ");
  char input_att[20];
  scanf("%s", input_att);
  
  if (strcmp(input_att, "artist") == 0){
    printf("Enter a artist: ");
    char artist[32];
    scanf(" %[^\n]%*c", artist);
    strcpy(songs[index].artist, artist);
  } 
  else if (strcmp(input_att, "title") == 0){
    printf("Enter a title: ");
    char artist[32];
    scanf(" %[^\n]%*c", artist);
    strcpy(songs[index].title, artist);
  }
  else if (strcmp(input_att, "duration") == 0){
    printf("Enter duration (minute): ");
    int min;
    scanf(" %d", &min);
    songs[index].minute = min;
    printf("Enter duration (second): ");
    int sec;
    scanf(" %d", &sec);
    songs[index].second = sec;
  }
  else if (strcmp(input_att, "danceability") == 0){
    printf("Enter danceability: ");
    float dan;
    scanf(" %.2f", &dan);
    songs[index].danceability = dan;
  }
  else{
    printf("Invalid Attribute given\n");
    return 0;
  }

  print_songs(songs);
  

  return 0;

}



