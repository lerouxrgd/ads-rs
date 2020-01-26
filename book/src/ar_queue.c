// An array-based queue

#include <stdio.h>
#include <stdlib.h>

typedef int item_t;

typedef struct {
  item_t *base;
  int front;
  int rear;
  int size;
} queue_t;

queue_t *create_queue(int size) {
  queue_t *qu;
  qu = (queue_t *)malloc(sizeof(queue_t));
  qu->base = (item_t *)malloc(size * sizeof(item_t));
  qu->size = size;
  qu->front = qu->rear = 0;
  return (qu);
}

int queue_empty(queue_t *qu) { return (qu->front == qu->rear); }

int enqueue(item_t x, queue_t *qu) {
  if (qu->front != ((qu->rear + 2) % qu->size)) {
    qu->base[qu->rear] = x;
    qu->rear = ((qu->rear + 1) % qu->size);
    return (0);
  } else
    return (-1);
}

item_t dequeue(queue_t *qu) {
  int tmp;
  tmp = qu->front;
  qu->front = ((qu->front + 1) % qu->size);
  return (qu->base[tmp]);
}

item_t front_element(queue_t *qu) { return (qu->base[qu->front]); }

void remove_queue(queue_t *qu) {
  free(qu->base);
  free(qu);
}

int main() {
  queue_t *qu;
  char nextop;
  qu = create_queue(50);
  printf("Made Array-Based Queue of size 50\n");
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
