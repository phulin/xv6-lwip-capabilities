#include "lwip/sockets.h"
#include "user.h"
#include "fcntl.h"
#include "stat.h"

#define MAX_REQ_LINE  20
#define MAX_LINE_LEN  200
#define FCACHE_SIZE   1024

int
strncmp(char *s1, char *s2, int len)
{
  int i;
  i=0;
  while (*s1 && *s1 == *s2)
  {
    i++;
    if (i == len)
      break;
    s1++, s2++;
  }
  if (i == len)
    return 0;
  return *s1 - *s2;
}

int lineptr = 0;
int linepos = 0;
int finished = 0;

char **line;

void
parsedata(char *start, char *end)
{
  int movelen = end-start+1;
  if (linepos + movelen > MAX_LINE_LEN)
  {
    movelen = MAX_LINE_LEN - linepos;
    *(start + movelen - 1) = '\0';
  }
  if (!finished)
  {
    memmove(line[lineptr]+linepos, start, movelen);
    finished = 1;
  } else {
    memmove(line[lineptr], start, movelen);
  }
  lineptr++;
  linepos = 0;
}

void
itoa(int val, char *data)
{
  char buffer[40];
  int i = -1;
  while (val != 0)
  {
    i++;
    buffer[i] = '0' + val % 10;
    val /= 10;
  }
  if (i == -1)
  {
    i++;
    buffer[i] = '0';
  }
  int j;
  for (j=0; j<=i; j++)
  {
    data[i-j] = buffer[j];
  }
  data[i+1] = '\0';
}

void
parse_request(int client)
{
  printf(1, "Request line count: %d\n", lineptr);
//  printf(1, "Request content:\n");
  char data[FCACHE_SIZE];
  int i;
  for (i=0; i<lineptr; i++)
  {
    char *start = line[i];
    while (*start == ' ') start++;
    if (!*start)
      continue;
//    printf(1, "%s\n", start);
    if (strncmp(start, "GET ", 4) == 0)
    {
//      printf(1, "GET request\n");
      char *head = start + 4;
      if (*head)
      {
        char old;
        char *end = head + 1;
        while ((*end != ' ') && (*end != '\0'))
          end++;
        old = *end;
        *end = '\0';
//        printf(1, " URI: %s\n", head);
        int f = open(head, O_RDONLY);
        int len;
        if (f > 0)
        {
          struct stat st;
          fstat(f, &st);
          char *message = "HTTP/1.1 200 OK\r\nContent-Length: ";
          send(client, message, strlen(message), 0);
          char buff[40];
          itoa(st.size, buff);
          send(client, buff, strlen(buff), 0);
          message = "\r\n\r\n";
          send(client, message, strlen(message), 0);
          while ((len = read(f, data, sizeof(data))) > 0)
          {
            send(client, data, len, 0);
          }
          close(f);
        } else {
          char *message = "HTTP/1.1 404 Not Found\r\n\r\nNot Found\r\n";
          send(client, message, strlen(message), 0);
        }
        *end = old;
      } else {
        char *message = "HTTP/1.1 400 Bad Request\r\n\r\nBad Request\r\n";
        send(client, message, strlen(message), 0);
      }
    }
  }
  printf(1, "End of request_parse\n");
}

int main()
{
  int i;
  line = malloc(MAX_REQ_LINE * sizeof(char *));
  for (i=0; i<MAX_REQ_LINE; i++)
    line[i] = malloc(MAX_LINE_LEN);
  int s = socket(PF_INET, SOCK_STREAM, 0);
  if (s<0)
    return 1;
  struct sockaddr_in addr;
  int addrlen = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(80);
  addr.sin_addr.s_addr = inet_addr("192.168.1.1");
  if (bind(s, &addr, sizeof(addr))<0)
    return 1;
  if (listen(s, 10)<0)
    return 1;
  int client;
  unsigned char data[1500];
  int len;
  char fdata[1500];
  while (client = accept(s, &addr, &addrlen))
  {
    int pid = fork();
    if (pid)
      continue;
    printf(1, "forked\n");
    lineptr = 0;
    char lastchr = '\0', curchr;
    while ((len = recv(client, data, sizeof(data), 0)) > 0)
    {
      printf(1, "PACK received\n");
        char *old = data;
        int i;
        int reqend = 0;
        for (i=0; i<len; i++)
        {
          curchr = data[i];
          if ((data[i] == '\0') || (data[i] == '\n') || (data[i] == '\r'))
          {
            data[i] = '\0';
//            printf(1, "%d: %s\n", i, old);
//            printf(1, "%d %d\n", lastchr, curchr);
            if (old+1 < &data[i])
              parsedata(old, &data[i]);
            if ((lastchr == '\n') && (curchr == '\r'))
            {
              i++;
              reqend = 1;
            }
            if ((lastchr == '\n') && (curchr == '\n'))
              reqend = 1;

            if (reqend)
            {
              parse_request(client);
              reqend = 0;
              lineptr = 0;
              linepos = 0;
              lastchr = '\0';
              curchr = '\0';
            }
            old = &data[i+1];
          }
          lastchr = curchr;
        }
        if ((i >= len) || ((data[i] != '\0') && (data[i] != '\r') && (data[i] != '\n')))
        {
          if (old < data + len)
          {
            finished = 0;
            lineptr++;
            linepos = (int)&data[0] - (int)old + len;
            memmove(line[lineptr], old, (int)data+len-(int)old);
          }
        }
    }
    sockclose(client);
    return 0;
  }
  sockclose(s);
  return 0;
}

