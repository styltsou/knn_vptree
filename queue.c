#include "queue.h"

#include <stdlib.h>

struct PQueue *newNode(int id, double priority) {
  struct PQueue *temp = (struct PQueue *)malloc(sizeof(struct PQueue));
  temp->id = id;
  temp->priority = priority;
  temp->next = NULL;

  return temp;
}

int q_size(struct PQueue **head) {
  struct PQueue *temp = *head;
  int count = 0;

  while (temp != NULL) {
    count++;
    temp = temp->next;
  }

  return count;
}

struct PQueue *peek(struct PQueue **head) {
  return *head;
}

void dequeue(struct PQueue **head) {
  struct PQueue *temp = *head;
  (*head) = (*head)->next;
  free(temp);
}

void enqueue(struct PQueue **head, int id, double priority) {
  struct PQueue *start = (*head);

  struct PQueue *temp = newNode(id, priority);

  if ((*head)->priority < priority) {
    temp->next = *head;
    (*head) = temp;
  } else {
    while (start->next != NULL && start->next->priority > priority) {
      start = start->next;
    }

    temp->next = start->next;
    start->next = temp;
  }
}