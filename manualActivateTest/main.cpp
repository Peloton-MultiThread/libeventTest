#include "threads.h"

bool MANUAL_TEST = true;

void* startCaller(void* arg) {
  usleep(3000000); // sleep for 5 seconds
  CalleeThread *calleeThread = static_cast<CalleeThread*>(arg);
  CallerThread callerThread(calleeThread);
  if (MANUAL_TEST) {
    callerThread.manualCall();
  } else {
    callerThread.pipeCall();
  }
}

int main(){
  CalleeThread calleeThread;
  pthread_t pid;
  pthread_create(&pid, nullptr, startCaller, &calleeThread);
  calleeThread.launch();
  void* thread_result;
  int res;
  res = pthread_join(pid, &thread_result);
  if (res != 0) {
    std::cout << "thread join fails" << std::endl;
    exit(1);
  }
}

