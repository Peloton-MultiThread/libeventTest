#include <memory>
#include <thread>
#include "threads.h"

void startWorker(WorkerThread *workerThread) {
  workerThread->launch();
}
int main() {
  std::shared_ptr<WorkerThread> workerThread = std::make_shared<WorkerThread>();
  std::shared_ptr<MasterThread> masterThread = std::make_shared<MasterThread>(workerThread.get());

  // workerThread runs in a seperate thread
  std::thread worker_thread(startWorker, workerThread.get());

  // masterThread runs in main thread, listening on 11223
  masterThread->launch();

  worker_thread.join();
}