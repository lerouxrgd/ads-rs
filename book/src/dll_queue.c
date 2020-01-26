// A doubly linked list-based queue

#include <stdio.h>
#include <stdlib.h>

typedef int item_t;

typedef struct qu_t {
  item_t item;
  struct qu_t *next;
  struct qu_t *previous;
} queue_t;

typedef queue_t node_t;

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

queue_t *create_queue() {
  queue_t *entrypoint;
  entrypoint = (queue_t *)malloc(sizeof(queue_t));
  entrypoint->next = entrypoint;
  entrypoint->previous = entrypoint;
  return (entrypoint);
}

int queue_empty(queue_t *qu) { return (qu->next == qu); }

void enqueue(item_t x, queue_t *qu) {
  queue_t *new;
  new = get_node();
  new->item = x;
  new->next = qu->next;
  qu->next = new;
  new->next->previous = new;
  new->previous = qu;
}

item_t dequeue(queue_t *qu) {
  queue_t *tmp;
  item_t tmp_item;
  tmp = qu->previous;
  tmp_item = tmp->item;
  tmp->previous->next = qu;
  qu->previous = tmp->previous;
  return_node(tmp);
  return (tmp_item);
}

item_t front_element(queue_t *qu) { return (qu->previous->item); }

void remove_queue(queue_t *qu) {
  queue_t *tmp;
  qu->previous->next = NULL;
  do {
    tmp = qu->next;
    return_node(qu);
    qu = tmp;
  } while (qu != NULL);
}

int main() {
  queue_t *qu;
  char nextop;
  qu = create_queue();
  printf("Made Doubly Linked List-Based Queue\n");
  while ((nextop = getchar()) != 'q') {
    if (nextop == 'e') {
      int insitem;
      scanf(" %d", &insitem);
      enqueue(insitem, qu);
      printf(" enqueued %d. The current front item is %d\n", insitem,
             front_element(qu));
    }
    if (nextop == 'd') {
      int de_item;
      getchar();
      de_item = dequeue(qu);
      printf("  dequeued item %d", de_item);
      if (queue_empty(qu))
        printf(" the queue is now empty\n");
      else
        printf(" the front element is now %d\n", front_element(qu));
    }
    if (nextop == '?') {
      getchar();
      if (queue_empty(qu))
        printf("the queue is empty\n");
      else
        printf("the front element is %d\n", front_element(qu));
    }
  }
  remove_queue(qu);
  printf(" removed queue\n");
  return (0);
}
