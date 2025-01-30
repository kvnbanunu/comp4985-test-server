#include "../include/setup.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>

typedef struct data_t
{
    int                     fd;
    volatile __sig_atomic_t running;
    struct sockaddr_in      addr;
    socklen_t               addr_len;
} data_t;

void setup(data_t *d, char s[INET_ADDRSTRLEN]);

int main(void)
{
    data_t data = {0};
    char   address_str[INET_ADDRSTRLEN];

    setup(&data, address_str);

    exit(EXIT_SUCCESS);
}

void setup(data_t *d, char s[INET_ADDRSTRLEN])
{
    find_address(&(d->addr.sin_addr.s_addr), s);
    d->addr_len = sizeof(struct sockaddr_in);
    d->fd       = setup_server(&(d->addr));
    find_port(&(d->addr), s);
    d->running = 1;
}
