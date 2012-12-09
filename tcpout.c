#include "user.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"

int main()
{
  printf(1, "Starting!\n");
  unsigned char data[512];
  int s;
  int len;
  s = socket(PF_INET, SOCK_STREAM, 0);
  printf(1, "About to set opt!\n");
  struct sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons(80);
  sa.sin_addr.s_addr = inet_addr("10.0.2.2");
  len = 1;
  printf(1, "About to set opt!\n");
  setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &len, sizeof(int));
  printf(1, "About to bind!\n");
  connect(s, (struct sockaddr *)&sa, sizeof(sa));
  char* msg = "Hello World";

  send(s, msg, sizeof(msg), 0);

  sockclose(s);
  return 0;
}
