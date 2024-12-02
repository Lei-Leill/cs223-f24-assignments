/*----------------------------------------------
 * Author: Lei Lei
 * Date: 11/29
 * Description: Implement function of searching keywords in given files
 ---------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define MAX_LINE_LENGTH 1024
#define MAX_FILES 1024

// Structure to hold thread arguments
typedef struct {
  int thread_id;
  int num_files;
  char **files;
  const char *keyword;
  int *match_count;
  pthread_mutex_t *print_mutex;
} thread_data;

void *search_files(void *arg) {
  thread_data *args = (thread_data *)arg;
  int local_count = 0;

  for (int i = 0; i < args->num_files; i++) {
    FILE *file = fopen(args->files[i], "r");
    if (!file) {
      pthread_mutex_lock(args->print_mutex);
      fprintf(stderr, "Thread %d: Failed to open file %s\n", args->thread_id, args->files[i]);
      pthread_mutex_unlock(args->print_mutex);
      continue;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
      if (strstr(line, args->keyword)) {
        local_count++;
        pthread_mutex_lock(args->print_mutex);
        printf("%d) %s: %s", args->thread_id, args->files[i], line);
        pthread_mutex_unlock(args->print_mutex);
      }
    }
    fclose(file);
  }

  *(args->match_count) = local_count;
  return NULL;
}

int get_files_from_command(const char *command, char **files) {
  /**
   * transform the find command to a list of files
   */
  FILE *pipe = popen(command, "r");
  if (!pipe) {
    perror("popen failed");
    return -1;
  }

  char line[MAX_LINE_LENGTH];
  int count = 0;
  while (fgets(line, sizeof(line), pipe) && count < MAX_FILES) {
    line[strcspn(line, "\n")] = '\0'; // Remove the newline character
    files[count] = strdup(line);
    count++;
  }
  pclose(pipe);
  return count;
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Usage: %s <NumThreads> <Keyword> '<Find Command>'\n", argv[0]);
    return 1;
  }

  int num_threads = atoi(argv[1]);
  const char *keyword = argv[2];
  const char *find_command = argv[3];

  if (num_threads <= 0) {
    fprintf(stderr, "Invalid number of threads.\n");
    return 1;
  }

  char *files[MAX_FILES];
  int num_files = get_files_from_command(find_command, files);
  if (num_files < 0) {
    return 1;
  }
  printf("Searching %d files for keyword: %s\n", num_files, keyword);

  double timer;
  struct timeval tstart, tend;
  gettimeofday(&tstart, NULL);

  pthread_t* threads = malloc(sizeof(pthread_t) * num_threads);
  thread_data* thread_args = malloc(sizeof(thread_data) * num_threads);
  int* match_counts = malloc(sizeof(int) *num_threads);
  pthread_mutex_t print_mutex;

  pthread_mutex_init(&print_mutex, NULL);

  int files_per_thread = num_files / num_threads;
  int remainder = num_files % num_threads;

  int file_index = 0;
  for (int i = 0; i < num_threads; i++) {
    int thread_files = files_per_thread + (i < remainder ? 1 : 0);
    printf("Thread %d searching %d files (%d to %d)\n", i, thread_files, file_index, file_index + thread_files -1);
    thread_args[i].thread_id = i;
    thread_args[i].num_files = thread_files;
    thread_args[i].files = &files[file_index];
    thread_args[i].keyword = keyword;
    thread_args[i].match_count = &match_counts[i];
    thread_args[i].print_mutex = &print_mutex;
    file_index += thread_files;

    pthread_create(&threads[i], NULL, search_files, &thread_args[i]);
  }

  int total_matches = 0;
  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
    printf("Thread %d found %d lines containing keyword %s\n", i, match_counts[i], keyword);
    total_matches += match_counts[i];
  }

  pthread_mutex_destroy(&print_mutex);

  gettimeofday(&tend, NULL);
  timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/ 1.e6;
  printf("Time Elapsed: %f seconds\n", timer);

  for (int i = 0; i < num_files; i++) {
    free(files[i]);
  }

  printf("Total matches: %d\n", total_matches);
  free(threads);
  free(thread_args);
  free(match_counts);
  return 0;
}
