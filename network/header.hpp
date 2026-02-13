#include <arpa/inet.h>
#include <fcntl.h>
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
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
using namespace std;

static const int BUFF = 1024;

// header.hpp
class conninfo {
 public:
  conninfo(int f) : fd(f) {}
  conninfo() = default;
  void adddata(char* buffer) { data += buffer; }
  void print() { printf("%d data: %s\n", fd, data.c_str()); }

 private:
  int fd;
  string data;
  int offset = 0;
};

class ConnPool {
 public:
  ConnPool(int e) { epfd = e; };
  void addmember(int fd) { pool[fd] = move(conninfo(fd)); }
  void write(int fd) {
    bzero(buffer, BUFF);
    int n = recv(fd, buffer, BUFF, 0);
    auto conn = &pool[fd];
    if (n == 0) {
      close(fd);
      epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
      pool.erase(fd);
      return;
    }
    if (n < 0 && errno == EAGAIN) {
      return;
    }
    conn->print();
    conn->adddata(buffer);
  }

 private:
  static int epfd;
  map<int, conninfo> pool;
  char buffer[BUFF];
};