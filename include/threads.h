#ifndef LIBEVENTTEST_THREADS_H
#define LIBEVENTTEST_THREADS_H

#include <evutil.h>
#include <cstdio>
#include <cstdlib>
#include <strings.h>
#include <unistd.h>
#include <iostream>
#include "event.h"

void master_event_cb(evutil_socket_t, short, void *);
void client_event_cb(evutil_socket_t, short, void *);

class WorkerThread {
 public:
  WorkerThread();
  void launch();
  int send_fd_;
  int recv_fd_;
  struct event_base *eb;
  struct event *event_;
  event_callback_fn master_event_callback_ptr;
  event_callback_fn client_event_callback_ptr;
};

class MasterThread {
 public:
  inline MasterThread(WorkerThread* worker_thread) : worker_thread_(worker_thread) {};
  void launch();
  void notifyWorker(int fd);
  WorkerThread *worker_thread_;
};

#endif //LIBEVENTTEST_THREADS_H
