/*----------------------------------------------
 * Author: Lei Lei
 * Date: 11/08
 * Description: Outputs a PPM image of the mandelbrot set. 
 ---------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "read_ppm.h"
#include "write_ppm.h"

int main(int argc, char* argv[]) {
  int size = 480;
  float xmin = -2.0;
  float xmax = 0.47;
  float ymin = -1.12;
  float ymax = 1.12;
  int maxIterations = 1000;

  int opt;
  while ((opt = getopt(argc, argv, ":s:l:r:t:b:")) != -1) {
    switch (opt) {
      case 's': size = atoi(optarg); break;
      case 'l': xmin = atof(optarg); break;
      case 'r': xmax = atof(optarg); break;
      case 't': ymax = atof(optarg); break;
      case 'b': ymin = atof(optarg); break;
      case '?': printf("usage: %s -s <size> -l <xmin> -r <xmax> -b <ymin> -t <ymax>\n", argv[0]); break;
    }
  }
  printf("Generating mandelbrot with size %dx%d\n", size, size);
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

  struct timeval tstart, tend;
  gettimeofday(&tstart, NULL);
  // generate and fill color to the board
  struct ppm_pixel* board = malloc(sizeof(struct ppm_pixel) * size * size);
  for (int i = 0; i < size; i ++){
    for (int j = 0; j < size; j ++){
      double xfrac = (double) i / size;
      double yfrac = (double) j / size;
      double x0 = xmin + xfrac * (xmax - xmin);
      double y0 = ymin + yfrac * (ymax - ymin);

      double x = 0;
      double y = 0;
      int iter = 0;
      while (iter < maxIterations && x*x + y*y < 2*2){
        double xtmp = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = xtmp;
        iter++;
      }
      int index = j * size + i;
      if (iter < maxIterations){
        board[index] = pallete[iter];
      } else{
        board[index].red = 0;
        board[index].green = 0;
        board[index].blue = 0;
      }
    }
  }
  gettimeofday(&tend, NULL);
  double time_taken = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;
  printf("Computed mandelbrot set (%dx%d) in %f seconds\n", size, size, time_taken);
  char filename[64];
  snprintf(filename, sizeof(filename), "mandelbrot-%d-%ld.ppm", size, time(0));
  printf("Writing file: %s\n", filename);
  write_ppm(filename, board, size, size);
  free(pallete);
  free(board);
  return 0;
}
