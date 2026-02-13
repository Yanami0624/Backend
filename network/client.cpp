// client
#include "header.hpp"
#define TEXTLEN 16
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 16555

void req(int seq) {
  struct sockaddr_in servaddr;
  char buff[TEXTLEN];
  bzero(buff, sizeof(buff));
  int sockfd;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr);
  servaddr.sin_port = htons(SERVER_PORT);
  connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

  int lenslice = rand() % (TEXTLEN - 2) + 2;
  char* slice = (char*)malloc(lenslice);
  memset(slice, 'a' + seq, lenslice);

  int off = 0;
  while (off < TEXTLEN) {
    int step = min(lenslice, TEXTLEN - lenslice);
    send(sockfd, slice, step, 0);
    off += lenslice;
    sleep(2);
  }
  close(sockfd);
}
int main() {
  const int nthread = 2;
  vector<thread> threads;
  srand(time(0));
  int seed = rand();
  for (int i = 0; i < nthread; ++i) {
    threads.emplace_back([=]() { req((seed + i) % nthread); });
  }
  for (int i = 0; i < nthread; ++i) {
    threads[i].join();
  }
  return 0;
}