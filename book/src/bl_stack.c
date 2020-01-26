#include <stdio.h>
#include <stdlib.h>

typedef int item_t;

typedef struct st_t {
  item_t *base;
  item_t *top;
  int size;
  struct st_t *previous;
} stack_t;

stack_t *create_stack(int size) {
  stack_t *st;
  st = (stack_t *)malloc(sizeof(stack_t));
  st->base = (item_t *)malloc(size * sizeof(item_t));
  st->size = size;
  st->top = st->base;
  st->previous = NULL;
  return (st);
}

int stack_empty(stack_t *st) {
  return (st->base == st->top && st->previous == NULL);
}

void push(item_t x, stack_t *st) {
  if (st->top < st->base + st->size) {
    *(st->top) = x;
    st->top += 1;
  } else {
    stack_t *new;
    new = (stack_t *)malloc(sizeof(stack_t));
    new->base = st->base;
    new->top = st->top;
    new->size = st->size;
    new->previous = st->previous;
    st->previous = new;
    st->base = (item_t *)malloc(st->size * sizeof(item_t));
    st->top = st->base + 1;
    *(st->base) = x;
  }
}

item_t pop(stack_t *st) {
  if (st->top == st->base) {
    stack_t *old;
    old = st->previous;
    st->previous = old->previous;
    free(st->base);
    st->base = old->base;
    st->top = old->top;
    st->size = old->size;
    free(old);
  }
  st->top -= 1;
  return (*(st->top));
}

item_t top_element(stack_t *st) {
  if (st->top == st->base)
    return (*(st->previous->top - 1));
  else
    return (*(st->top - 1));
}

void remove_stack(stack_t *st) {
  stack_t *tmp;
  do {
    tmp = st->previous;
    free(st->base);
    free(st);
    st = tmp;
  } while (st != NULL);
}

int main() {
  stack_t *st;
  char nextop;
  st = create_stack(5);
  printf("Made Block List-Based Stack with blocks of size 5\n");
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
