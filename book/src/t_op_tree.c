// A top-down optimal search tree

#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 256

typedef int object_t;
typedef int key_t;
typedef struct tr_n_t {
  key_t key;
  struct tr_n_t *left;
  struct tr_n_t *right;
} tree_node_t;

tree_node_t *currentblock = NULL;
int size_left;
tree_node_t *free_list = NULL;

tree_node_t *get_node() {
  tree_node_t *tmp;
  if (free_list != NULL) {
    tmp = free_list;
    free_list = free_list->left;
  } else {
    if (currentblock == NULL || size_left == 0) {
      currentblock = (tree_node_t *)malloc(BLOCKSIZE * sizeof(tree_node_t));
      size_left = BLOCKSIZE;
    }
    tmp = currentblock++;
    size_left -= 1;
  }
  return (tmp);
}

void return_node(tree_node_t *node) {
  node->left = free_list;
  free_list = node;
}

tree_node_t *make_tree(tree_node_t *list) {
  typedef struct {
    tree_node_t *node1;
    tree_node_t *node2;
    int number;
  } st_item;
  st_item current, left, right;
  st_item stack[100];
  int st_p = 0;
  tree_node_t *tmp, *root;
  int length = 0;
  for (tmp = list; tmp != NULL; tmp = tmp->right)
    length += 1; /* find length of list */

  root = get_node();
  current.node1 = root; /* put root node on stack */
  current.node2 = NULL;
  current.number = length; /* root expands to length leaves */
  stack[st_p++] = current;
  while (st_p > 0) /* there is still unexpanded node */
  {
    current = stack[--st_p];
    if (current.number > 1) /* create (empty) tree nodes */
    {
      left.node1 = get_node();
      left.node2 = current.node2;
      left.number = current.number / 2;
      right.node1 = get_node();
      right.node2 = current.node1;
      right.number = current.number - left.number;
      (current.node1)->left = left.node1;
      (current.node1)->right = right.node1;
      stack[st_p++] = right;
      stack[st_p++] = left;
    } else /* reached a leaf, must be filled with list item */
    {
      (current.node1)->left = list->left; /* fill leaf */
      (current.node1)->key = list->key;   /* from list */
      (current.node1)->right = NULL;
      if (current.node2 != NULL)
        /* insert comparison key in interior node */
        (current.node2)->key = list->key;
      tmp = list;         /* unlink first item from list */
      list = list->right; /* content has been copied to */
      return_node(tmp);   /* leaf, so node is returned */
    }
  }
  return (root);
}

void check_tree(tree_node_t *tr, int depth, int lower, int upper) {
  if (tr->key < lower || tr->key >= upper)
    printf("Wrong Key Order \n");
  if (tr->right == NULL) {
    if (*((int *)tr->left) == tr->key * tr->key)
      printf("%d(%d)  ", tr->key, depth);
    else
      printf("Wrong Object \n");
  } else {
    check_tree(tr->left, depth + 1, lower, tr->key);
    check_tree(tr->right, depth + 1, tr->key, upper);
  }
}

tree_node_t *make_list(tree_node_t *tree) {
  tree_node_t *list, *node;
  if (tree->left == NULL) {
    return_node(tree);
    return (NULL);
  } else {
    tree_node_t *stack[100];
    int st_p = 0;
    stack[st_p++] = tree;
    list = NULL;
    while (st_p > 0) {
      node = stack[--st_p];
      if (node->right == NULL) {
        node->right = list;
        list = node;
      } else {
        stack[st_p++] = node->left;
        stack[st_p++] = node->right;
        return_node(node);
      }
    }
    return (list);
  }
}

int main() {
  tree_node_t *search_tree, *tmp_node, *list;
  int *insobj;
  int i, j;
  list = NULL;
  for (i = 11; i > 0; i--) {
    tmp_node = get_node();
    tmp_node->right = list;
    insobj = (int *)malloc(sizeof(int));
    *insobj = i * i;
    tmp_node->left = (tree_node_t *)insobj;
    tmp_node->key = i;
    list = tmp_node;
  }
  printf("Made Sorted List\n");
  tmp_node = list;
  while (tmp_node != NULL) {
    printf(" %d", tmp_node->key);
    tmp_node = tmp_node->right;
  }
  search_tree = make_tree(list);
  printf("\n Made Top-Down Optimal Tree\n");
  check_tree(search_tree, 0, -1000, 1000);
  printf("\n Finished Testing Tree\n");
  list = make_list(search_tree);
  printf("Converted Back to Sorted List\n");
  tmp_node = list;
  while (tmp_node != NULL) {
    printf(" %d", tmp_node->key);
    tmp_node = tmp_node->right;
  }
  printf("\n");
  return (0);
}
