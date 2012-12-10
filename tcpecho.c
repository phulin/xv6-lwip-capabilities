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
  sa.sin_addr.s_addr = inet_addr("10.0.2.15");
  len = 1;
  printf(1, "About to set opt!\n");
  setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &len, sizeof(int));
  printf(1, "About to bind!\n");
  bind(s, (struct sockaddr *)&sa, sizeof(sa));
  uint addrlen = (uint)sizeof(sa);

  printf(1, "About to listen!\n");
  int client;
  listen(s, 1);
  printf(1, "About to enter loop!\n");
  while ((client = accept(s, (struct sockaddr *)&sa, &addrlen)) > 0)
  {
    printf(1, "Looping!");
      do {
        len = recv(client, data, sizeof(data), 0);
        printf(1, "received %d bytes\n", len);
        send(client, data, len, 0);
        if (data[0] == '!')
            len = -1;
      } while (len > 0);
      sockclose(client);
  }
  sockclose(s);
  return 0;
}
