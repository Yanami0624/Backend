#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>
using namespace std;
#define BUFFSIZE 16
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 16555

// client.cpp
void req(int seq) {
  struct sockaddr_in servaddr;
  char buff[BUFFSIZE];
  bzero(buff, sizeof(buff));
  int sockfd;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr);
  servaddr.sin_port = htons(SERVER_PORT);
  connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

  int lenslice = rand() % BUFFSIZE;
  // lenslice = 1;
  char* slice = (char*)malloc(lenslice);
  memset(slice, 'a' + seq, lenslice);
  int off = 0;
  while (off < BUFFSIZE) {
    lenslice = off + lenslice > BUFFSIZE ? BUFFSIZE - off : lenslice;
    strncpy(buff, slice, lenslice);
    int n = send(sockfd, buff, lenslice, 0);
    off += n;
    usleep(10000);
  }
  send(sockfd, "\n", 1, 0);

  close(sockfd);
}
int main() {
  const int nthread = 26;
  vector<thread> threads;
  for (int i = 0; i < nthread; ++i) {
    threads.emplace_back([=]() { req(i); });
  }
  for (int i = 0; i < nthread; ++i) {
    threads[i].join();
  }
  return 0;
}