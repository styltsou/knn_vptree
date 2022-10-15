#ifndef QUEUE_H
#define QUEUE_H

struct PQueue {
  int id;
  double priority;
  struct PQueue *next;
};

struct PQueue *newNode(int id, double priority);
void dequeue(struct PQueue **head);
void enqueue(struct PQueue **head, int id, double priority);
struct PQueue *peek(struct PQueue **head);
int q_size(struct PQueue **head);

#endif