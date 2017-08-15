#include <iostream>
#include <unistd.h>
#include "event.h"
#include "event2/thread.h"
#include <pthread.h>

#define CALL_NUM 2

extern bool MANUAL_TEST;

class CalleeThread {
 private:
  struct event_base *libevent_base_;
  struct event *event_;
  int send_fd_;
  int recv_fd_;
 public:
  CalleeThread();
  void launch();
  struct event_base *getEventBase() {
    return libevent_base_;
  }
  struct event *getEvent() {
    return event_;
  }
  inline int get_send_fd() {
    return send_fd_;
  }
  inline int get_recv_fd() {
    return recv_fd_;
  }
  inline struct event* get_event() {
    return event_;
  }
};

class CallerThread {
 private:
  CalleeThread *callee_thread_;
 public:
  CallerThread(CalleeThread * calleeThread): callee_thread_(calleeThread){};
  void pipeCall();
  void manualCall();
};