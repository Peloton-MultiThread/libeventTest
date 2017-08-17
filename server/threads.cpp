#include "threads.h"
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#define LISTEN_PORT 11223
#define BUFSIZE 2


/* Set the socket to non-blocking mode */
inline void SetNonBlocking(evutil_socket_t fd) {
  auto flags = fcntl(fd, F_GETFL);
  flags |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flags) < 0) {
  }
}

/* Set TCP No Delay for lower latency */
inline void SetTCPNoDelay(evutil_socket_t fd) {
  int one = 1;
  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof one);
}
// read content from client, print them out and count how many times the WorkerThread is triggered
void client_event_cb(evutil_socket_t fd, short evflags, void *args) {
  int static count = 0;
  WorkerThread *wt = static_cast<WorkerThread*>(args);
  char buf[BUFSIZE];

  int read_bytes;
  while ((read_bytes = read(fd, buf, BUFSIZE)) > 0) {

    buf[read_bytes] = '\0';
    count++;
    std::cout << "reading: " << read_bytes << ", " << buf << ", count: " << count << ", src: " << fd << ", evflags: "
              << evflags << std::endl;
  }
  if (read_bytes < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      std::cout << "pipe empty" << std::endl;
    }
  }
  if (read_bytes == 0) {
    std::cout << "finish reading, close the event" << std::endl;
    event_del(wt->event_);
    event_free(wt->event_);
    close(fd);
  }
  std::cout << "end of callback" << std::endl;
}

// read connected fd from pipe and add event associated with the fd
void master_event_cb(evutil_socket_t fd, short evflags, void *args) {
  std::cout << "master_event_cb" << std::endl;
  WorkerThread *wt = static_cast<WorkerThread*>(args);
  int client_fd = 0;
  std::cout << "worker starts reading from pipe" << std::endl;
  if (read((wt->recv_fd_), (char*)&client_fd, sizeof(client_fd)) < 0) {
    std::cout << "Can't read from master-worker pipe";
    exit(1);
  }
  std::cout << "worker read from pipe, " << client_fd << std::endl;

  SetNonBlocking(client_fd);
  SetTCPNoDelay(client_fd);

  wt->event_ = event_new(wt->eb, client_fd, EV_READ | EV_PERSIST,  client_event_cb, wt);
  event_add(wt->event_, 0);
  std::cout << "add client event onto worker thread's eb" << std::endl;
}

// start running as a server
void MasterThread::launch() {
  int parentfd, optval, childfd;
  socklen_t clientlen;
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  parentfd = socket(AF_INET, SOCK_STREAM, 0);
  if (parentfd < 0) {
    std::cout << "ERROR opening socket";
    exit(1);
  }
  /* allows us to restart server immediately */
  optval = 1;
  setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR,
             (const void *)&optval , sizeof(int));

  /* bind port to socket */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons(LISTEN_PORT);
  if (bind(parentfd, (struct sockaddr *) &serveraddr,
           sizeof(serveraddr)) < 0) {
    std::cout << "ERROR on binding";
    exit(1);
  }
  /* get us ready to accept connection requests */
  if (listen(parentfd, 5) < 0) { /* allow 5 requests to queue up */
    std::cout << "ERROR on listen";
    exit(1);
  }
  childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
  std::cout << "master receives connection " << childfd << std::endl;
  notifyWorker(childfd);
//  while(1);
}

// write connected fd into workerThread's pipe
void MasterThread::notifyWorker(int fd) {
  write(worker_thread_->send_fd_, (char*)&fd, sizeof(fd));
  std::cout << "Master write fd into pipe " << fd << std::endl;
}

WorkerThread::WorkerThread() {
  int fds[2];
  if (pipe(fds)) {
    std::cout << "Can't create notify pipe to accept connections" << std::endl;
    exit(1);
  }
  send_fd_ = fds[1];
  recv_fd_ = fds[0];
  eb = event_base_new();
  event* event = event_new(eb, recv_fd_, EV_READ | EV_PERSIST,
                                   master_event_cb, this);
  event_add(event, 0);
  std::cout << "Worker thread constructor, pipe: " << send_fd_ << ", " << recv_fd_ << std::endl;
}

// worker starts its event looping
void WorkerThread::launch() {
  std::cout << "Worker thread dispatch loop" << std::endl;
  event_base_dispatch(eb);
}

