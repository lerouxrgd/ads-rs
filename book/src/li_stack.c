#include <stdio.h>
#include <stdlib.h>

typedef int item_t;
typedef struct st_t {
  item_t item;
  struct st_t *next;
} stack_t;

typedef stack_t node_t;

#define BLOCKSIZE 256

node_t *currentblock = NULL;
int size_left;
node_t *free_list = NULL;

node_t *get_node() {
  node_t *tmp;
  if (free_list != NULL) {
    tmp = free_list;
    free_list = free_list->next;
  } else {
    if (currentblock == NULL || size_left == 0) {
      currentblock = (node_t *)malloc(BLOCKSIZE * sizeof(node_t));
      size_left = BLOCKSIZE;
    }
    tmp = currentblock++;
    size_left -= 1;
  }
  return (tmp);
}

void return_node(node_t *node) {
  node->next = free_list;
  free_list = node;
}

stack_t *create_stack(void) {
  stack_t *st;
  st = get_node();
  st->next = NULL;
  return (st);
}

int stack_empty(stack_t *st) { return (st->next == NULL); }

void push(item_t x, stack_t *st) {
  stack_t *tmp;
  tmp = get_node();
  tmp->item = x;
  tmp->next = st->next;
  st->next = tmp;
}

item_t pop(stack_t *st) {
  stack_t *tmp;
  item_t tmp_item;
  tmp = st->next;
  st->next = tmp->next;
  tmp_item = tmp->item;
  return_node(tmp);
  return (tmp_item);
}

item_t top_element(stack_t *st) { return (st->next->item); }

void remove_stack(stack_t *st) {
  stack_t *tmp;
  do {
    tmp = st->next;
    return_node(st);
    st = tmp;
  } while (tmp != NULL);
}

int main() {
  stack_t *st;
  char nextop;
  st = create_stack();
  printf("Made Linked List-Based Stack\n");
  while ((nextop = getchar()) != 'q') {
    if (nextop == 'i') {
      int insitem;
      scanf(" %d", &insitem);
      push(insitem, st);
      printf(" pushed %d. The current top item is %d\n", insitem,
             top_element(st));
    }
    if (nextop == 'd') {
      int de_item;
      getchar();
      de_item = pop(st);
      printf("  popped item %d", de_item);
      if (stack_empty(st))
        printf(" the stack is now empty\n");
      else
        printf(" the top element is now %d\n", top_element(st));
    }
    if (nextop == '?') {
      getchar();
      if (stack_empty(st))
        printf("the stack is empty\n");
      else
        printf("the top element is %d\n", top_element(st));
    }
  }
  remove_stack(st);
  printf(" removed stack\n");
  return (0);
}
