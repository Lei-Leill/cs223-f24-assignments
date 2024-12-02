/*----------------------------------------------
 * Author: Lei Lei
 * Date: 11/29
 * Description: Implement dependency of files using tree
 ---------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "tree.h"
#include <time.h>
#include <sys/time.h>

#define MAX_FILES 1024
#define MAX_LINE 256

pthread_mutex_t tree_mutex = PTHREAD_MUTEX_INITIALIZER;

// Thread data structure
typedef struct {
    char** files;
    int start;
    int end;
    struct tree_node** root;
} thread_data_t;


// Parse a single file and print its dependencies
void parse_and_print_dependencies(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("%s not found\n", filename);
        return;
    }

    printf("%s has the following dependencies:\n", filename);

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        char* include = strstr(line, "#include");
        if (include) {
            char dependency[MAX_LINE];
            if (sscanf(include, "#include \"%[^\"]\"", dependency) == 1 || 
                sscanf(include, "#include <%[^>]> ", dependency) == 1) {
                printf("  %s\n", dependency);
            }
        }
    }

    fclose(file);
}


// Thread function
void* thread_function(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    for (int i = data->start; i < data->end; ++i) {
        FILE* file = fopen(data->files[i], "r");
        if (!file) {
            printf("Warning: Could not open file: %s\n", data->files[i]);
            continue; // Skip this file if it cannot be opened
        }
        fclose(file); // Close the file after checking validity
        struct tree_node* node = find(data->files[i], *data->root);
        pthread_mutex_lock(&tree_mutex);
        if (!node) { // If not in the tree
            *data->root = insert(data->files[i], *data->root); // Insert file into the tree
        }
        pthread_mutex_unlock(&tree_mutex);
    }
    return NULL;
}

// Get files using the provided command
int get_files_from_command(const char* command, char** files) {
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        perror("popen failed");
        return -1;
    }

    char line[MAX_LINE];
    int count = 0;
    while (fgets(line, sizeof(line), pipe) && count < MAX_FILES) {
        line[strcspn(line, "\n")] = '\0'; // Remove the newline character
        const char* dot = strrchr(line, '.');
        if (dot && (strcmp(dot, ".h") == 0 || strcmp(dot, ".cpp") == 0)) {
            if (strncmp(line, "./", 2) == 0) {
                files[count] = strdup(line + 2); // Remove "./" prefix
            } else {
                files[count] = strdup(line);
            }
            count++;
        }
    }
    pclose(pipe);
    return count;
}

// Main function
int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <num_threads> <find_command>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    if (num_threads <= 0) {
        fprintf(stderr, "Invalid number of threads. Must be greater than 0.\n");
        return 1;
    }

    char* files[MAX_FILES];
    int file_count = get_files_from_command(argv[2], files);
    if (file_count < 0) {
        fprintf(stderr, "Failed to process the find command.\n");
        return 1;
    }

    printf("Processing %d files\n", file_count);

    struct tree_node* root = NULL;

    double timer;
    struct timeval tstart, tend;
    gettimeofday(&tstart, NULL);
    pthread_t* threads = malloc(sizeof(pthread_t) * num_threads);
    thread_data_t* thread_data = malloc(sizeof(thread_data_t) * num_threads);

    int files_per_thread = file_count / num_threads;
    int extra_files = file_count % num_threads;

    int start = 0;
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].files = files;
        thread_data[i].start = start;
        thread_data[i].end = start + files_per_thread + (i < extra_files ? 1 : 0);
        thread_data[i].root = &root;
        start = thread_data[i].end;
        printf("Thread %d processing %d files (arguments %d to %d)\n",
           i, thread_data[i].end - thread_data[i].start,
           thread_data[i].start + 2, thread_data[i].end + 1);
        pthread_create(&threads[i], NULL, thread_function, &thread_data[i]);

    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&tend, NULL);
    timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/ 1.e6;
    printf("Time Elapsed: %f seconds\n", timer);

    char command[MAX_LINE];
    while (1) {
        printf("$ ");
        if (!fgets(command, sizeof(command), stdin)) {
            break;
        }

        command[strcspn(command, "\n")] = '\0'; // Remove newline

        if (strcmp(command, "quit") == 0) {
            break;
        } else if (strcmp(command, "list") == 0) {
            printSorted(root);
        } else {
            parse_and_print_dependencies(command);
        }
    }

    for (int i = 0; i < file_count; i++) {
        free(files[i]);
    }
    clear(root);
    free(threads);
    free(thread_data);
    return 0;
}