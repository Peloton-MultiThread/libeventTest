#include "threads.h"


void manual_cb(evutil_socket_t socket, short ev_flag, void* arg){
  static int count = 0;
  count++;
  std::cout << "callee activated, count: " << count << std::endl;
  usleep(10000000);
  std::cout << "exit manual_cb count: " << count << std::endl;
}

void pipe_cb(evutil_socket_t socket, short ev_flag, void* arg) {
  char buf[1];
  static int count = 0;
  CalleeThread * calleeThread = static_cast<CalleeThread*>(arg);
  if (read(calleeThread->get_recv_fd(), buf, 1) < 0) {
    std::cout << "read error" << std::endl;
  }
  count++;
  std::cout << "callee " << socket << " receives: " << buf[0] << ", count: " << count << std::endl;
  usleep(1000000);
}

CalleeThread::CalleeThread() {
  int fds[2];
  if (pipe(fds) < 0) {
    std::cout << "error constructing pipe" << std::endl;
  }
  send_fd_ = fds[1];
  recv_fd_ = fds[0];
  std::cout << "send_fd_: " << send_fd_ << ", recv_fd_: " << recv_fd_ << std::endl;
}

void CalleeThread::launch() {
  /* We must specify this function if we use multiple threads*/
  evthread_use_pthreads();
  libevent_base_ = event_base_new();
  evthread_make_base_notifiable(libevent_base_);
  if(MANUAL_TEST) {
    event_ = event_new(libevent_base_, recv_fd_, EV_READ | EV_PERSIST , manual_cb, nullptr);
  } else {
    event_ = event_new(libevent_base_, recv_fd_, EV_READ | EV_PERSIST, pipe_cb, this);
  }
  event_add(event_, 0);
  std::cout << "callee thread adds event" << std::endl;
  std::cout << "callee thread starts running" << std::endl;
  event_base_dispatch(libevent_base_);
}

void CallerThread::manualCall() {
  int i = 0;
  while (i < CALL_NUM) {
    event_active(callee_thread_->getEvent(), EV_WRITE, 0);
    std::cout << "[caller] call " << i << " times" << std::endl;
    i++;
  }
}

void CallerThread::pipeCall() {
  char buf[1];
  buf[0] = 'A';
  int i = 0;
  while (i < CALL_NUM) {
    if (write(callee_thread_->get_send_fd(), buf, 1) < 0) {
      std::cout << "write error" << std::endl;
      exit(1);
    }
    std::cout << "[caller] call " << i << " times" << std::endl;
    i++;
  }
}