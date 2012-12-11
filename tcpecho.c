#include "user.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"

int main()
{
  unsigned char data[512];
  int s;
  int len;
  s = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons(80);
  sa.sin_addr.s_addr = inet_addr("10.0.2.15");
  len = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &len, sizeof(int));
  bind(s, (struct sockaddr *)&sa, sizeof(sa));
  uint addrlen = (uint)sizeof(sa);

  int client;
  listen(s, 1);
  while ((client = accept(s, (struct sockaddr *)&sa, &addrlen)) > 0)
  {
      do {
        len = recv(client, data, sizeof(data), 0);
        send(client, data, len, 0);
        if (data[0] == '!')
            len = -1;
      } while (len > 0);
    sockclose(client);
  }
  sockclose(s);
  return 0;
}
