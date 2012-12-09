#include "../types.h"
#include "../defs.h"
#include "../fs.h"
#include "../file.h"
#include "../traps.h"
#include "ne.h"
#include "eth.h"
#include "lwip/pbuf.h"

static void (*if_input)(struct netif*);
static ne_t ne;
static struct netif* netif;

void
ethintr()
{
  ne_interrupt(&ne);
  (*if_input)(netif);
  return;
}

// not used because ioctl() isn't still implemented.
int
ethioctl(struct inode* ip, int request, void* p)
{
  switch (request) {
  case ETH_IPC_SETUP:
    //cprintf("%s: ETH_IPC_SETUP isn't still implemented because of no IPC\n", ne.name);
    break;
  }
  return 0;
}

int
ethread(struct inode* ip, char* p, int n)
{
  return ne_pio_read(&ne, (uchar*)p, n);
}

int
ethwrite(struct inode* ip, char* p, int n)
{
  return ne_pio_write(&ne, (uchar*)p, n);
}

void
ethinit(void (*input_handler)(struct netif *netif))
{
  int i;
  char name[] = "eth#";
  int ports[] = { 0x300, 0xC100, 0x240, 0x280, 0x320, 0x340, 0x360 };

  devsw[ETHERNET].write = ethwrite;
  devsw[ETHERNET].read = ethread;
  devsw[ETHERNET].ioctl = ethioctl;

  for (i = 0; i < NELEM(ports); ++i) {
    cprintf("Ethernet: Initialize port %d [0x%x].\n", i, ports[i]);
    memset(&ne, 0, sizeof(ne));
    name[3] = '0' + i;
    strncpy(ne.name, name, strlen(name)+1);
    ne.irq = IRQ_ETH;
    ne.base = ports[i];
    if (ne_probe(&ne)) {
      ne_init(&ne);
      picenable(ne.irq);
      ioapicenable(ne.irq, 0);
      break;
    }
  }
  if_input = input_handler;

  return;
}

