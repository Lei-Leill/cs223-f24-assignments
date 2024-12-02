#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include "tree.h" // Assumes the tree library with basic insert, find, and printSorted functions

#define MAX_LINE_LENGTH 1024
#define MAX_FILES 1024
#define MAX_LINE 256

// Mutex for thread-safe operations
pthread_mutex_t tree_mutex = PTHREAD_MUTEX_INITIALIZER;

// Thread data structure
typedef struct {
  char** files;
  int start;
  int end;
  struct tree_node** root;
} thread_data_t;

// Check if a file is valid
int is_valid_file(const char* filename) {
  struct stat buffer;
  return (stat(filename, &buffer) == 0);
}

// Parse dependencies in a file
void parse_file(const char* filename, struct tree_node** root) {
  FILE* file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Could not open file: %s\n", filename);
    return;
  }

  pthread_mutex_lock(&tree_mutex);
  *root = insert(filename, *root); // Add the file to the tree
  pthread_mutex_unlock(&tree_mutex);

  char line[MAX_LINE];
  while (fgets(line, sizeof(line), file)) {
    char* include = strstr(line, "#include");
    if (include) {
      char* start = strchr(include, '\"');
      char* end = start ? strchr(start + 1, '\"') : NULL;
      if (start && end) {
        char dependency[MAX_LINE];
        snprintf(dependency, end - start, "%s", start + 1);
        pthread_mutex_lock(&tree_mutex);
        *root = insert(dependency, *root); // Insert dependency as a separate node
        pthread_mutex_unlock(&tree_mutex);
      }
    }
  }

  fclose(file);
}

// Thread function
void* thread_function(void* arg) {
  thread_data_t* data = (thread_data_t*)arg;

  for (int i = data->start; i < data->end; ++i) {
    if (is_valid_file(data->files[i])) {
      parse_file(data->files[i], data->root);
    }
  }

  return NULL;
}

// Interactive prompt
void interactive_prompt(struct tree_node* root) {
  char input[MAX_LINE];
  while (1) {
    printf("$ ");
    if (!fgets(input, sizeof(input), stdin)) {
      break;
    }

    // Remove trailing newline
    input[strcspn(input, "\n")] = '\0';

    if (strcmp(input, "quit") == 0) {
      break;
    } else if (strcmp(input, "list") == 0) {
      printSorted(root); // Print all nodes in alphabetical order
    } else {
      struct tree_node* node = find(input, root);
      if (!node) {
        printf("%s not found\n", input);
      } else {
        printf("%s is present in the tree.\n", input);
      }
    }
  }
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

int main(int argc, char* argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <num_threads> <file1> <file2> ...\n", argv[0]);
    return EXIT_FAILURE;
  }

  int num_threads = atoi(argv[1]);
  const char *find_command = argv[2];

  if (num_threads <= 0) {
    fprintf(stderr, "Invalid number of threads.\n");
    return 1;
  }

  char *files[MAX_FILES];
  int num_files = get_files_from_command(find_command, files);
  if (num_files < 0) {
    return 1;
  }


  struct tree_node* root = NULL;
  pthread_t* threads = malloc(sizeof(pthread_t) * num_threads);
  thread_data_t* thread_data = malloc(sizeof(thread_data_t) * num_threads);

  int files_per_thread = (num_files + num_threads - 1) / num_threads;

  printf("Processing %d files\n", num_files);

  for (int i = 0; i < num_threads; ++i) {
    thread_data[i].files = files;
    thread_data[i].start = i * files_per_thread;
    thread_data[i].end = (i + 1) * files_per_thread;
    if (thread_data[i].end > num_files) {
      thread_data[i].end = num_files;
    }
    thread_data[i].root = &root;

    printf("Thread %d processing %d files (arguments %d to %d)\n",
           i, thread_data[i].end - thread_data[i].start,
           thread_data[i].start + 2, thread_data[i].end + 1);

    pthread_create(&threads[i], NULL, thread_function, &thread_data[i]);
  }

  for (int i = 0; i < num_threads; ++i) {
    pthread_join(threads[i], NULL);
  }

  printf("Elapsed time is TBD\n"); // Replace with a proper timer if needed

  interactive_prompt(root);

  clear(root);
  pthread_mutex_destroy(&tree_mutex);

  return EXIT_SUCCESS;
}
