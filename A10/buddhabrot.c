/*----------------------------------------------
 * Author: Lei Lei
 * Date: 11/13
 * Description: Outputs a PPM image of the budhabrot set using four threads 
 ---------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include "read_ppm.h"
#include "write_ppm.h"

int* member_array;
int* count_array;
int max_count = 0;  
pthread_barrier_t barrier;
pthread_mutex_t mutex;


struct thread_data {
  int row_start, row_end, col_start, col_end;
  struct ppm_pixel* board;
  int size, maxIterations;
  float xmin, xmax, ymin, ymax;
  pthread_t* id;
};

void* thread_func(void* args){
  struct thread_data* data = (struct thread_data*) args;
  printf("Thread # %lu sub-board block: cols (%i, %i) to rows (%i, %i)\n",
      *data->id, data->col_start, data->col_end, data->row_start, data->row_end);
  // start calculating the colors
  for (int r = data->col_start; r < data->col_end; r++) {
    for (int c = data->row_start; c < data->row_end; c++) {
      float xfrac = c / (float) data->size;
      float yfrac = r / (float) data->size;
      float x0 = data->xmin + xfrac * (data->xmax - data->xmin);
      float y0 = data->ymin + yfrac * (data->ymax - data->ymin);
      float x = 0.0;
      float y = 0.0;
      int iter = 0;
      while (iter < data->maxIterations && x * x + y * y < 2 * 2) {
        float xtmp = x * x - y * y + x0;
        y = 2 * x * y + y0;
        x = xtmp;
        iter++;
      }
      int index = r * data->size + c;
      if (iter < data->maxIterations) { // escaped
        member_array[index] = 0;
      } else { // within the range
        member_array[index] = 1;
      }
    }
  }

  // compute the count and max count
  int* this_count = malloc(sizeof(int) * data->size * data->size);
  for (int i = 0; i < data->size * data->size; i++) {
    this_count[i] = 0;
  }
  int this_max_count = 0;
  
  for (int r = data->col_start; r < data->col_end; r++) {
    for (int c = data->row_start; c < data->row_end; c++) {
      int index = r * data->size + c;
      if (member_array[index]){
        continue;
      }
      float xfrac = c / (float) data->size;
      float yfrac = r / (float) data->size;
      float x0 = data->xmin + xfrac * (data->xmax - data->xmin);
      float y0 = data->ymin + yfrac * (data->ymax - data->ymin);
      float x = 0.0;
      float y = 0.0;
      int count = 0;
      while (x * x + y * y < 2 * 2) {
        float xtmp = x * x - y * y + x0;
        y = 2 * x * y + y0;
        x = xtmp;
        int yrow = (int)(data->size * (y - data->ymin)/(data->ymax - data->ymin));
        int xcol = (int)(data->size * (x - data->xmin)/(data->xmax - data->xmin));
        if (yrow < 0 || yrow >= data->size) {continue;} // out of range
        if (xcol < 0 || xcol >= data->size) {continue;} // out of range

        int index = yrow * data->size + xcol;
        this_count[index]++;
        if (this_count[index] > this_max_count) {
          this_max_count = this_count[index];
        }
      }
    }
  }
  // update max count and count
  pthread_mutex_lock(&mutex);
  for (int r = 0; r < data->size; r++) {
    for (int c = 0; c < data->size; c++) {
      int index = r * data->size + c;
      count_array[index] += this_count[index];
    }
  }
  if (max_count < this_max_count) {
    max_count = this_max_count;
  }
  pthread_mutex_unlock(&mutex);
  free(this_count);
  pthread_barrier_wait(&barrier);
  // compute colors
  float gamma = 0.681;
  float factor = 1.0 / gamma;
  for (int r = data->col_start; r < data->col_end; r++) {
    for (int c = data->row_start; c < data->row_end; c++) {
      float value = 0;
      int index = r * data->size + c;
      if (count_array[index] > 0) {
        value = log(count_array[index]) / log(max_count);
        value = pow(value, factor);
      }
      data->board[index].red = (int) (value * 255);
      data->board[index].green = (int) (value * 255);
      data->board[index].blue = (int) (value * 255);
    }
  }
  printf("Thread # %lu finished\n", *data->id);
  return NULL;
}


int main(int argc, char* argv[]) {
  int size = 480;
  float xmin = -2.0;
  float xmax = 0.47;
  float ymin = -1.12;
  float ymax = 1.12;
  int maxIterations = 1000;
  int numProcesses = 4;

  int opt;
  while ((opt = getopt(argc, argv, ":s:l:r:t:b:p:")) != -1) {
    switch (opt) {
      case 's': size = atoi(optarg); break;
      case 'l': xmin = atof(optarg); break;
      case 'r': xmax = atof(optarg); break;
      case 't': ymax = atof(optarg); break;
      case 'b': ymin = atof(optarg); break;
      case '?': printf("usage: %s -s <size> -l <xmin> -r <xmax> "
        "-b <ymin> -t <ymax> -p <numProcesses>\n", argv[0]); break;
    }
  }
  printf("Generating buddhabrot with size %dx%d\n", size, size);
  printf("  Num processes = %d\n", numProcesses);
  printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
  printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);

  member_array = malloc(sizeof(int) * size * size);
  count_array = malloc(sizeof(int) * size * size);
  for (int i = 0; i < size * size; i ++){
    count_array[i] = 0;
  }

  double timer;
  struct timeval tstart, tend;
  gettimeofday(&tstart, NULL);

  struct ppm_pixel* board = malloc(sizeof(struct ppm_pixel) * size * size);
  pthread_t threads[4];
  struct thread_data data[4];
  pthread_barrier_init(&barrier, NULL, 4);
  pthread_mutex_init(&mutex, NULL);


  for(int i = 0; i < 4; i ++){
    data[i].maxIterations = maxIterations;
    data[i].size = size;
    data[i].board = board;
    data[i].xmin = xmin;
    data[i].xmax = xmax;
    data[i].ymin = ymin;
    data[i].ymax = ymax;
    //data[i].pallete = pallete;
    data[i].id = &threads[i];
    if (i <2){
      data[i].row_start = 0;
      data[i].row_end = size/2;
    }
    else{
      data[i].row_start = size/2;
      data[i].row_end = size;
    }
    if (i % 2 == 1){
      data[i].col_start = 0;
      data[i].col_end = size/2;
    }
    else{
      data[i].col_start = size/2;
      data[i].col_end = size;
    }
    pthread_create(&threads[i], NULL, thread_func, &data[i]);
    
  }

  for(int i = 0; i < 4; i ++){
    pthread_join(threads[i], NULL);
  }
  gettimeofday(&tend, NULL);
  timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/ 1.e6;
  printf("Computed mandelbrot set (%dx%d) in %f seconds\n", size, size, timer);
  char filename[64];
  snprintf(filename, sizeof(filename), "buddhabrot-%d-%ld.ppm", size, time(0));
  printf("Writing file: %s\n", filename);
  write_ppm(filename, board, size, size);

  free(member_array);
  free(count_array);
  free(board);
  return 0;
  
}
