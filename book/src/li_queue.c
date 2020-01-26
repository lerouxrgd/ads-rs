// A list-based queue

#include <stdio.h>
#include <stdlib.h>

typedef int item_t;
typedef struct qu_n_t {
  item_t item;
  struct qu_n_t *next;
} qu_node_t;

typedef struct {
  qu_node_t *remove;
  qu_node_t *insert;
} queue_t;
typedef qu_node_t node_t;

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
  queue_t *qu;
  qu = (queue_t *)malloc(sizeof(queue_t));
  qu->remove = qu->insert = NULL;
  return (qu);
}

int queue_empty(queue_t *qu) { return (qu->insert == NULL); }

void enqueue(item_t x, queue_t *qu) {
  qu_node_t *tmp;
  tmp = get_node();
  tmp->item = x;
  tmp->next = NULL;       /* end marker */
  if (qu->insert != NULL) /* queue nonempty */
  {
    qu->insert->next = tmp;
    qu->insert = tmp;
  } else /* insert in empty queue */
  {
    qu->remove = qu->insert = tmp;
  }
}

item_t dequeue(queue_t *qu) {
  qu_node_t *tmp;
  item_t tmp_item;
  tmp = qu->remove;
  tmp_item = tmp->item;
  qu->remove = tmp->next;
  if (qu->remove == NULL) /* reached end */
    qu->insert = NULL;    /* make queue empty */
  return_node(tmp);
  return (tmp_item);
}

item_t front_element(queue_t *qu) { return (qu->remove->item); }

void remove_queue(queue_t *qu) {
  qu_node_t *tmp;
  while (qu->remove != NULL) {
    tmp = qu->remove;
    qu->remove = tmp->next;
    return_node(tmp);
  }
  free(qu);
}

int main() {
  queue_t *qu;
  char nextop;
  qu = create_queue();
  printf("Made List-Based Queue\n");
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
