#include <iostream>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>

#define BUF_MAX 2
#define COUNT 20000

int main() {
  std::cout << "Hello, World!" << std::endl;

  // from csapp
  int clientfd;
  struct hostent *hp;
  struct sockaddr_in serveraddr;
  if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return 01;
  }
  if ((hp = gethostbyname("127.0.0.1")) == NULL) {
    return -2;
  }
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  bcopy((char *)hp->h_addr_list[0], (char *)&serveraddr.sin_addr, hp->h_length);
  serveraddr.sin_port = htons(11223);
  if (connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
    std::cout << "fail to connect" << std::endl;
    return -1;
  }
  int i = 0;
  while(i < COUNT) {
    char buf[BUF_MAX];
    int j = 0;
    for (j = 0; j < BUF_MAX; j++) {
      buf[j] = 'A' + i % 26;
    }
    //buf[BUF_MAX - 1] = '\0';
    send(clientfd, buf, BUF_MAX, 0);
    i++;
    std::cout << "send: " << i << ", " << buf << std::endl;
  }
  shutdown(clientfd, SHUT_RDWR);
  close(clientfd);
  return 0;
}