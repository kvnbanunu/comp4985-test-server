#include "../include/setup.h"
#include "../include/asn.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

typedef struct data_t
{
    int                     fd;
    int cfd; /* client fd */
    struct sockaddr_in      addr;
    socklen_t               addr_len;
    pthread_t thread;
} data_t;

typedef struct thread_data_t
{
    int fd;
    int cfd;
} thread_data_t;

static volatile sig_atomic_t running = 1;

static void setup(data_t *d, char s[INET_ADDRSTRLEN]);
static void setup_sig_handler(void);
static void sig_handler(int sig);
static void cleanup(data_t *d);
static void process_req(data_t *d);
static void send_sys_success(uint8_t buf[], int fd, uint8_t packet_type);
static void send_sys_error(uint8_t buf[], int fd, int err);
static void send_acc_login_success(uint8_t buf[], int fd, uint16_t user_id);

int main(void)
{
    data_t data = {0};
    char   address_str[INET_ADDRSTRLEN];
    int retval = EXIT_SUCCESS;

    setup(&data, address_str);

    while (running)
    {
        data.cfd = accept(data.fd, NULL, 0);
        if (data.cfd < 0)
        {
            if (!running)
            {
                break;
            }
            continue;
        }
    }

cleanup:
    cleanup(&data);
    exit(retval);
}

static void setup(data_t *d, char s[INET_ADDRSTRLEN])
{
    find_address(&(d->addr.sin_addr.s_addr), s);
    d->addr_len = sizeof(struct sockaddr_in);
    d->fd       = setup_server(&(d->addr));
    find_port(&(d->addr), s);
}

/* Pairs SIGINT with sig_handler */
static void setup_sig_handler(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif
    sa.sa_handler = sig_handler;
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if(sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* Write to stdout a shutdown message and set exit_flag to end while loop in main */
static void sig_handler(int sig)
{
    const char *message = "\nSIGINT received. Server shutting down.\n";
    write(1, message, strlen(message));
    running = 0;
}

#pragma GCC diagnostic pop

static void cleanup(data_t *d)
{
    close(d->fd);
    if(d->cfd < 0)
    {
        close(d->cfd);
    }
}

static void process_req(data_t *d)
{
    header_t header = {0};
    uint8_t buf[PACKETLEN];
    int result;

    result = decode_packet(buf, &header);
    switch(result)
    {
    }
}

static void send_sys_success(uint8_t buf[], int fd, uint8_t packet_type)
{
    int len = encode_sys_success_res(buf, packet_type);
    write(fd, buf, len);
}

static void send_sys_error(uint8_t buf[], int fd, int err)
{
    int len = 0;
    uint8_t errcode = 0;
    switch(err)
    {
        case -1:
            errcode = EC_GENSERVER;
            break;
        case -2:
            errcode = EC_GENSERVER;
            break;
        case -3:
            errcode = EC_GENSERVER;
            break;
        case -4:
            errcode = EC_GENSERVER;
            break;
        case -5:
            errcode = EC_GENSERVER;
            break;
        default:
            errcode = EC_GENSERVER;
    }
    len = encode_sys_error_res(buf, errcode);
}

static void send_acc_login_success(uint8_t buf[], int fd, uint16_t user_id)
{
    int len = encode_acc_login_success_res(buf, user_id);
    write(fd, buf, len);
}
