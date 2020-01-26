// An array-based stack

#include <stdio.h>
#include <stdlib.h>

typedef int item_t;

typedef struct {
  item_t *base;
  item_t *top;
  int size;
} stack_t;

stack_t *create_stack(int size) {
  stack_t *st;
  st = (stack_t *)malloc(sizeof(stack_t));
  st->base = (item_t *)malloc(size * sizeof(item_t));
  st->size = size;
  st->top = st->base;
  return (st);
}

int stack_empty(stack_t *st) { return (st->base == st->top); }

int push(item_t x, stack_t *st) {
  if (st->top < st->base + st->size) {
    *(st->top) = x;
    st->top += 1;
    return (0);
  } else
    return (-1);
}

item_t pop(stack_t *st) {
  st->top -= 1;
  return (*(st->top));
}

item_t top_element(stack_t *st) { return (*(st->top - 1)); }

void remove_stack(stack_t *st) {
  free(st->base);
  free(st);
}

int main() {
  stack_t *st;
  char nextop;
  st = create_stack(50);
  printf("Made Array-Based Stack of size 50\n");
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
