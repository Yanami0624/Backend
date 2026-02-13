// header.hpp
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
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
using namespace std;

const long MAGIC = 0x11451419;
class packet {
 public:
  packet(char* buffer) { len = strlen(buffer); }
  packet(int l) : len(l) {};
  bool valid() { return header == MAGIC; }
  int getlen() { return len; }
  const int headersize = sizeof(packet);

 private:
  long header = MAGIC;
  long len = 0;
};

static const int BUFF = 2048;
int myrecv(int fd, char* buffer) {
  packet* p = (packet*)malloc(sizeof(packet));
  recv(fd, (char*)p, sizeof(packet), 0);
  if (!p->valid()) return -1;
  int off = 0;
  int len = p->getlen();
  while (off < len) {
    int n = recv(fd, buffer + off, len - off, 0);
    off += n;
  }
  return 0;
}

int mysend(int fd, char* buffer) {
  packet p(strlen(buffer));
  send(fd, (char*)&p, sizeof(packet), 0);
  int off = 0;
  int len = p.getlen();
  while (off < len) {
    int n = send(fd, buffer + off, len - off, 0);
    off += n;
  }
  return 0;
}