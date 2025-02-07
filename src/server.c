#include "../include/asn.h"
#include "../include/setup.h"
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct data_t
{
    int                fd;
    int                cfd; /* client fd */
    struct sockaddr_in addr;
    socklen_t          addr_len;
} data_t;

static volatile sig_atomic_t running = 1;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

static void setup(data_t *d, char s[INET_ADDRSTRLEN]);
static void setup_sig_handler(void);
static void sig_handler(int sig);
static void cleanup(const data_t *d);
static void process_req(const data_t *d);
static void send_sys_success(uint8_t buf[], int fd, uint8_t packet_type);
static void send_sys_error(uint8_t buf[], int fd, int err);
static void send_acc_login_success(uint8_t buf[], int fd, uint16_t user_id);

int main(void)
{
    data_t        data = {0};
    char          address_str[INET_ADDRSTRLEN];
    int           retval = EXIT_SUCCESS;
    struct pollfd fd;

    setup(&data, address_str);

    data.cfd = accept(data.fd, NULL, 0);
    if(data.cfd < 0)
    {
        retval = EXIT_FAILURE;
        goto cleanup;
    }
    fd.fd     = data.cfd;
    fd.events = POLLIN;

    while(running)
    {
        if(poll(&fd, 1, -1) == -1)
        {
            break;
        }
        process_req(&data);
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
    setup_sig_handler();
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

static void cleanup(const data_t *d)
{
    close(d->fd);
    if(d->cfd > 0)
    {
        close(d->cfd);
    }
}

static void process_req(const data_t *d)
{
    header_t header = {0};
    uint8_t  buf[PACKETLEN];
    int      result;

    read(d->cfd, buf, HEADERLEN);
    decode_header(buf, &header);

    read(d->cfd, buf + HEADERLEN, header.payload_len);
    result = decode_packet(buf, &header);

    memset(buf, 0, PACKETLEN);
    if(result < 0)
    {
        send_sys_error(buf, d->cfd, result);
    }

    if(header.packet_type == ACC_LOGIN)
    {
        send_acc_login_success(buf, d->cfd, 1); /* 1 for testing only */
    }

    if(header.packet_type == ACC_LOGOUT || header.packet_type == ACC_CREATE || header.packet_type == ACC_EDIT)
    {
        send_sys_success(buf, d->cfd, header.packet_type);
    }
}

static void send_sys_success(uint8_t buf[], int fd, uint8_t packet_type)
{
    int len = encode_sys_success_res(buf, packet_type);
    write(fd, buf, (size_t)len);
}

static void send_sys_error(uint8_t buf[], int fd, int err)
{
    int len = 0;
    len     = encode_sys_error_res(buf, err);
    write(fd, buf, (size_t)len);
}

static void send_acc_login_success(uint8_t buf[], int fd, uint16_t user_id)
{
    int len = encode_acc_login_success_res(buf, user_id);
    write(fd, buf, (size_t)len);
}
