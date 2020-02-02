// A bottom-up optimal search tree

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
  tree_node_t *end, *root;
  if (list == NULL) {
    root = get_node(); /* create empty tree */
    root->left = root->right = NULL;
    return (root);
  } else if (list->right == NULL)
    return (list); /* one-leaf tree */
  else             /* nontrivial work required: at least two nodes */
  {
    root = end = get_node(); /* convert input list into*/
    end->left = list;        /* leaves below new list  */
    end->key = list->key;
    list = list->right;
    end->left->right = NULL;
    while (list != NULL) {
      end->right = get_node();
      end = end->right;
      end->left = list;
      end->key = list->key;
      list = list->right;
      end->left->right = NULL;
    }
    end->right = NULL;
    /* end creating list of leaves */
    {
      tree_node_t *old_list, *new_list, *tmp1, *tmp2;
      old_list = root;
      while (old_list->right != NULL) { /* join first two trees from old_list */
        tmp1 = old_list;
        tmp2 = old_list->right;
        old_list = old_list->right->right;
        tmp2->right = tmp2->left;
        tmp2->left = tmp1->left;
        tmp1->left = tmp2;
        tmp1->right = NULL;
        new_list = end = tmp1;   /* new_list started*/
        while (old_list != NULL) /* not at end */
        {
          if (old_list->right == NULL) /*last tree*/
          {
            end->right = old_list;
            old_list = NULL;
          } else /* join next two trees of old_list */
          {
            tmp1 = old_list;
            tmp2 = old_list->right;
            old_list = old_list->right->right;
            tmp2->right = tmp2->left;
            tmp2->left = tmp1->left;
            tmp1->left = tmp2;
            tmp1->right = NULL;
            end->right = tmp1;
            end = end->right;
          }
        } /* finished one pass through old_list */
        old_list = new_list;
      } /* end joining pairs of trees together */
      root = old_list->left;
      return_node(old_list);
    }
    return (root);
  }
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
  printf("\n Made Bottom-Up Optimal Tree\n");
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
