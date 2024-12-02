/*#include "tree.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

struct tree_node* find(const char* name, struct tree_node* root)
{
  printf("Entered find() with name=\"%s\" and root=\'%p\'\n", name, root);
  if (root == NULL){
    return NULL;
  }
  if (strncmp(name, root->data.name,1) < 0){
    printf("left %s", root->data.name);
    return find(name, root->left);
  }
  if (strncmp(name, root->data.name,1) > 0){
    printf("right %s", root->data.name);
    return find(name, root->right);
  }
  return root;
}

struct tree_node* insert(const char* name, struct tree_node* root)
{
  if(root == NULL){
    root = malloc(sizeof(struct tree_node*));
    strcpy(root->data.name, name);
    root->left = NULL;
    root->right = NULL;
  }
  else if (strncmp(name, root->data.name, 1) < 0){
    root->left = insert(name, root->left);
  }
  else{
    root->right = insert(name, root->right);
  }
  return root;
}

void clear(struct tree_node* root)
{
  if (root == NULL) return;
  clear(root->left);
  clear(root->right);
  free(root);
}

void print(struct tree_node* root)
{
  if(root == NULL){
    return;
  }
  printf("%s\n", root->data.name);
  if (root->left != NULL){
    printf("  l:");
    print(root->left);
  }
  if (root->right != NULL){
    printf("  r:");
    print(root->right);
  }

}

void printSorted(struct tree_node* root)
{
}
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

// Helper function to create a new tree node
struct tree_node* create_node(const char* name) {
  struct tree_node* node = (struct tree_node*)malloc(sizeof(struct tree_node));
  if (!node) {
    perror("Failed to allocate memory");
    exit(EXIT_FAILURE);
  }
  strncpy(node->data.name, name, sizeof(node->data.name) - 1);
  node->data.name[sizeof(node->data.name) - 1] = '\0';
  node->left = node->right = NULL;
  return node;
}

struct tree_node* insert(const char* name, struct tree_node* root) {
  if (!root) {
    return create_node(name);
  }
  int cmp = strcmp(name, root->data.name);
  if (cmp < 0) {
    root->left = insert(name, root->left);
  } else if (cmp > 0) {
    root->right = insert(name, root->right);
  }
  return root;
}

struct tree_node* find(const char* name, struct tree_node* root) {
  if (!root) {
    return NULL;
  }
  int cmp = strcmp(name, root->data.name);
  if (cmp == 0) {
    return root;
  } else if (cmp < 0) {
    return find(name, root->left);
  } else {
    return find(name, root->right);
  }
}

void clear(struct tree_node* root) {
  if (root) {
    clear(root->left);
    clear(root->right);
    free(root);
  }
}

void print(struct tree_node* root) {
  if (root) {
    printf("%s\n", root->data.name);
    if(root->left != NULL){
      printf("  l:");
      print(root->left);
    }
    if(root->right != NULL){
      printf("  r:");
      print(root->right);
    }
  }
}

void printSorted(struct tree_node* root) {
  if (root) {
    printSorted(root->left);
    printf("%s\n", root->data.name);
    printSorted(root->right);
  }
}
