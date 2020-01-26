// An array-based stack with shadow copy

#include <stdio.h>
#include <stdlib.h>

typedef int item_t;

typedef struct {
  item_t *base;
  int size;
  int max_size;
  item_t *copy;
  int copy_size;
} stack_t;

stack_t *create_stack(int size) {
  stack_t *st;
  st = (stack_t *)malloc(sizeof(stack_t));
  st->base = (item_t *)malloc(size * sizeof(item_t));
  st->max_size = size;
  st->size = 0;
  st->copy = NULL;
  st->copy_size = 0;
  return (st);
}

int stack_empty(stack_t *st) { return (st->size == 0); }

void push(item_t x, stack_t *st) {
  *(st->base + st->size) = x;
  st->size += 1;
  if (st->copy != NULL ||
      st->size >= 0.75 * st->max_size) { /* have to continue or start copying */
    int additional_copies = 4;
    if (st->copy == NULL) /* start copying: allocate space */
    {
      st->copy = (item_t *)malloc(2 * st->max_size * sizeof(item_t));
    }
    /* continue copying: at most 4 items per push operation */
    while (additional_copies > 0 && st->copy_size < st->size) {
      *(st->copy + st->copy_size) = *(st->base + st->copy_size);
      st->copy_size += 1;
      additional_copies -= 1;
    }
    if (st->copy_size == st->size) /* copy complete */
    {
      free(st->base);
      st->base = st->copy;
      st->max_size *= 2;
      st->copy = NULL;
      st->copy_size = 0;
    }
  }
}

item_t pop(stack_t *st) {
  item_t tmp_item;
  st->size -= 1;
  tmp_item = *(st->base + st->size);
  if (st->copy_size == st->size) /* copy complete */
  {
    free(st->base);
    st->base = st->copy;
    st->max_size *= 2;
    st->copy = NULL;
    st->copy_size = 0;
  }
  return (tmp_item);
}

item_t top_element(stack_t *st) { return (*(st->base + st->size - 1)); }

void remove_stack(stack_t *st) {
  free(st->base);
  if (st->copy != NULL)
    free(st->copy);
  free(st);
}

void list_stack(stack_t *st) {
  int i;
  printf("The items currently on the stack are, from the top\n");
  for (i = st->size - 1; i >= 0; i--)
    printf("%d ", *(st->base + i));
  if (st->copy != NULL) {
    printf("A copy is being constructed, the items on the copy are\n");
    for (i = st->copy_size - 1; i >= 0; i--)
      printf("%d ", *(st->copy + i));
    printf("\n");
  } else
    printf("no copy exists now\n");
}

int main() {
  stack_t *st;
  char nextop;
  st = create_stack(3);
  printf("Made Stack\n");
  while ((nextop = getchar()) != 'q') {
    if (nextop == 'a') {
      int addkey;
      scanf(" %d", &addkey);
      push(addkey, st);
      printf("pushed item %d on stack\n", addkey);
    }
    if (nextop == 'p') {
      printf("popped item %d from stack\n", pop(st));
    }
    if (nextop == '?') {
      if (stack_empty(st))
        printf("The stack is empty\n");
      else {
        printf("The top item on the stack is %d,", top_element(st));
        printf("the stack size is %d\n", st->size);
        list_stack(st);
      }
    }
  }
  remove_stack(st);
  printf(" Removed stack.\n");
  return (0);
}
