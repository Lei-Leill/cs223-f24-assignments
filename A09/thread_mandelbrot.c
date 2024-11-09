/*----------------------------------------------
 * Author: Lei Lei
 * Date: 11/08
 * Description: Outputs a PPM image of the mandelbrot set using four threads 
 ---------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include "read_ppm.h"
#include "write_ppm.h"

struct thread_data {
  int row_start, row_end, col_start, col_end;
  struct ppm_pixel* board;
  struct ppm_pixel* pallete;
  int size, maxIterations;
  float xmin, xmax, ymin, ymax;
  pthread_t* id;
};

void* thread_func(void* id) {
  struct thread_data* data = (struct thread_data*) id;
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
      if (iter < data->maxIterations) { // diverge, paint color
        data->board[index].red = data->pallete[iter].red;
        data->board[index].green = data->pallete[iter].green;
        data->board[index].blue = data->pallete[iter].blue;
      } else { // converge, paint black
        data->board[index].red = 0;
        data->board[index].green = 0;
        data->board[index].blue = 0;
      }
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
  printf("Generating mandelbrot with size %dx%d\n", size, size);
  printf("  Num processes = %d\n", numProcesses);
  printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
  printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);

  // generate pallet
  struct ppm_pixel* pallete = malloc(sizeof(struct ppm_pixel) * maxIterations);
  srand(time(0));
  for (int i = 0; i < maxIterations; i ++){
    pallete[i].red = rand() % 255;
    pallete[i].green = rand() % 255;
    pallete[i].blue = rand() % 255;
  }

  struct ppm_pixel* board = malloc(sizeof(struct ppm_pixel) * size * size);

  double timer;
  struct timeval tstart, tend;
  gettimeofday(&tstart, NULL);
  // thread
  //pthread_t threads[4];
  //struct thread_data data[4];
  struct thread_data* data = malloc(sizeof(struct thread_data) * 4);
  pthread_t* threads = malloc(sizeof(pthread_t) * 4);
  for (int i = 0; i < 4; i ++){
    data[i].maxIterations = maxIterations;
    data[i].size = size;
    data[i].board = board;
    data[i].xmin = xmin;
    data[i].xmax = xmax;
    data[i].ymin = ymin;
    data[i].ymax = ymax;
    data[i].pallete = pallete;
    data[i].id = &threads[i];
    if (i % 2 == 0) {
      data[i].row_start = 0;
      data[i].row_end = size / 2;
    } else {
      data[i].row_start = size / 2;
      data[i].row_end = size;
    }
    if (i - 2 < 0) {
      data[i].col_start = 0;
      data[i].col_end = size / 2;
    } else {
      data[i].col_start = size / 2;
      data[i].col_end = size;
    }
    pthread_create(&threads[i], NULL, thread_func, &data[i]);
  }

  for (int i = 0; i < 4; i ++){
    pthread_join(threads[i], NULL);
  }
  gettimeofday(&tend, NULL);
  timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/ 1.e6;
  printf("Computed mandelbrot set (%dx%d) in %f seconds\n", size, size, timer);
  char filename[64];
  snprintf(filename, sizeof(filename), "mandelbrot-%d-%ld.ppm", size, time(0));
  printf("Writing file: %s\n", filename);
  write_ppm(filename, board, size, size);

  free(threads);
  free(data);
  free(board);
  free(pallete);
  return 0;
}
