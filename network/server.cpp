#include <netinet/in.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#define BUFFSIZE 2048
#define DEFAULT_PORT 16555
#define MAXLINK 2048

int sockfd;
struct sockaddr_in addr;
vector<string> recvbuffer;

void endhd(int pid) { exit(0); }

int main() {
  signal(SIGINT, endhd);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = 0;
  addr.sin_port = htons(DEFAULT_PORT);
  bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

  listen(sockfd, MAXLINK);

  auto epfd = epoll_create(114514);
  while (true) {
    int connfd = accept(sockfd, NULL, NULL);
    if (connfd > 0) {
      epoll_event ev;
      ev.events = EPOLLIN;
      ev.data.fd = connfd;
      epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
    }

    epoll_event events[1024];
    int n = epoll_wait(epfd, events, 114514, 10);
    for (int i = 0; i < n; ++i) {
      if (!(events[i].events & EPOLLIN)) continue;
      int fd = events[i].data.fd;
      char buf[1024];
      while (int n = recv(fd, buf, sizeof(buf), 0)) cout.write(buf, n);
    }
  }
}