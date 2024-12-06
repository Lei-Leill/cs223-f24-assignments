/*----------------------------------------------
 * Author: Lei Lei
 * Date: 12/04
 * Description: Implement malloc and free based on "My malloc: mylloc and mhysa" by
                Johan Montelius
 ---------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct chunk {
  int size;
  int used;
  struct chunk *next;
};
struct chunk *flist = NULL;

void *malloc (size_t size) {
  // TODO: Implement malloc with a free list (See reading for details)
  if (size == 0){
    return NULL;
  }
  struct chunk* curr = flist;
  struct chunk* prev = NULL;
  while (curr != NULL){
    if (curr->size >= (int)size){
      curr->used = (int)size;
      if (prev != NULL){
        prev->next = curr->next;
      }
      else{ //if curr is the first element
        flist = curr->next;
      }
      curr->next = NULL;
      return (void*) (curr + 1);
    }
    prev = curr;
    curr = curr->next;
  }
  // No matches 
  void* memory = sbrk(size + sizeof(struct chunk));
  if(memory != (void*)-1){
    struct chunk* cnk = (struct chunk*) (memory);
    cnk->size = size; 
    cnk->used = size;
    cnk->next = NULL;
    return (void*) (cnk + 1);
  }
  return NULL;
}

void free(void *memory) {
  // TODO: Implement malloc with a free list (See reading for details)
  if (memory != NULL){
    struct chunk* current = (struct chunk*)((struct chunk*)memory - 1);
    current->next = flist;
    flist = current;
  }
  return;
}
