// server
#include "header.hpp"

#define DEFAULT_PORT 16555
#define MAXLINK 2048

int sockfd;
struct sockaddr_in addr;

int main() {
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = 0;
  addr.sin_port = htons(DEFAULT_PORT);
  bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

  listen(sockfd, MAXLINK);

  auto epfd = epoll_create(1024);
  epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = sockfd;
  epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

  while (true) {
    epoll_event events[1024];
    int n = epoll_wait(epfd, events, 1024, 10);
    for (int i = 0; i < n; ++i) {
      int fd = events[i].data.fd;

      if ((events[i].events & EPOLLIN)) {
        if (fd == sockfd) {
          int connfd = accept(sockfd, NULL, NULL);
          // fcntl(connfd, F_SETFL, O_NONBLOCK);
          epoll_event ev;
          ev.events = EPOLLIN;
          ev.data.fd = connfd;
          epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
        } else {
          char buffer[BUFF];
          bzero(buffer, BUFF);
          myrecv(fd, buffer);
          printf("%s", buffer);
        }
      }

      if ((events[i].events & EPOLLHUP)) {
        // close(fd);
      }
    }
  }
}