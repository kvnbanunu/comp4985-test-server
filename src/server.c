#include "../include/asn.h"
#include "../include/setup.h"
#include <netinet/in.h>
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
static int  process_req(const data_t *d);
static void send_sys_success(uint8_t buf[], int fd, uint8_t packet_type);
static void send_sys_error(uint8_t buf[], int fd, int err);
static void send_acc_login_success(uint8_t buf[], int fd, uint16_t user_id);
static void send_cht_send(uint8_t buf[], int fd);

int main(void)
{
    data_t data = {0};
    char   address_str[INET_ADDRSTRLEN];
    int    retval = EXIT_SUCCESS;

    setup(&data, address_str);

    // This program is only used to test 1 client, so the program will end when the client connects or errors.
    data.cfd = accept(data.fd, NULL, 0);
    if(data.cfd < 0)
    {
        retval = EXIT_FAILURE;
        goto cleanup;
    }
    while(running)
    {
        if(data.cfd < 0)
        {
            if(running)
            {
                printf("Connection ended\n");
            }
            break;
        }
        if(process_req(&data) == -1)
        {
            printf("End of Read\n");
            break;
        }
    }

cleanup:
    close(data.fd);
    if(data.cfd > -1)
    {
        close(data.cfd);
    }
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

static int process_req(const data_t *d)
{
    header_t header = {0};
    uint8_t  buf[PACKETLEN];
    int      result;

    memset(buf, 0, PACKETLEN);
    if(read(d->cfd, buf, HEADERLEN) < HEADERLEN)
    {
        return -1;
    }
    decode_header(buf, &header);

    if(read(d->cfd, buf + HEADERLEN, header.payload_len) < header.payload_len)
    {
        return -1;
    }
    result = decode_packet(buf, &header);

    memset(buf, 0, PACKETLEN);
    if(result < 0)
    {
        send_sys_error(buf, d->cfd, result);
        return SYS_ERROR;
    }

    if(header.packet_type == ACC_LOGIN)
    {
        send_acc_login_success(buf, d->cfd, 1); /* 1 for testing only */
        return ACC_LOGIN_SUCCESS;
    }

    if(header.packet_type == ACC_CREATE || header.packet_type == ACC_EDIT)
    {
        send_sys_success(buf, d->cfd, header.packet_type);
        return SYS_SUCCESS;
    }

    if(header.packet_type == ACC_LOGOUT)
    {
        // no response
        return ACC_LOGOUT;
    }

    if(header.packet_type == CHT_SEND)
    {
        send_sys_success(buf, d->cfd, header.packet_type);

        // send an example of a chat message from another user.
        send_cht_send(buf, d->cfd);
        return CHT_SEND;
    }
    return -1;
}

static void send_sys_success(uint8_t buf[], int fd, uint8_t packet_type)
{
    int len = encode_sys_success_res(buf, packet_type);
    write(fd, buf, (size_t)len);
}

static void send_sys_error(uint8_t buf[], int fd, int err)
{
    int len = encode_sys_error_res(buf, err);
    write(fd, buf, (size_t)len);
}

static void send_acc_login_success(uint8_t buf[], int fd, uint16_t user_id)
{
    int len = encode_acc_login_success_res(buf, user_id);
    write(fd, buf, (size_t)len);
}

static void send_cht_send(uint8_t buf[], int fd)
{
    int len = encode_cht_send(buf);
    write(fd, buf, (size_t)len);
}
