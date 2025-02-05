#include "../include/asn.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int decode_uint8(uint8_t buf[], int pos)
{
    printf("uint8: %u\n", buf[pos]);
    return (int)sizeof(uint8_t);
}

static int decode_uint16(uint8_t buf[], int pos)
{
    uint16_t copy;
    memcpy(&copy, buf + pos, sizeof(uint16_t));
    printf("uint16: %u\n", ntohs(copy));
    return (int)sizeof(uint16_t);
}

/* following a ASN_INT tag, pos would land on len */
int decode_int(uint8_t buf[], int pos)
{
    int len = buf[pos];
    int res = 1;
    switch(len)
    {
        case 1:
            res += decode_uint8(buf, pos + 1);
            break;
        case 2:
            res += decode_uint16(buf, pos + 1);
            break;
        default:
            fprintf(stderr, "Invalid Integer Length: %d\n", len);
    }
    return res;
}

int decode_enum(uint8_t buf[], int pos)
{
    printf("Enum: %u\n", buf[pos + 1]);
    return 1;
}

/* following ASN_STR tag, pos would land on len */
int decode_str(uint8_t buf[], int pos)
{
    int len = buf[pos];
    char *msg = (char *)malloc(len + 1);
    msg[len] = '\0';
    printf("string: %s\n", msg);
    free(msg);
    return 1 + len;
}

int decode_time(uint8_t buf[], int pos)
{
    int len = buf[pos];
    char *time = (char *)malloc(len + 1);
    time[len] = '\0';
    printf("time: %s\n", time);
    free(time);
    return 1 + len;
}

void decode_header(uint8_t buf[], header_t *header)
{
    int pos = 0;
    int sender_id;
    int payload_len;

    memcpy(&header->packet_type, buf, sizeof(uint8_t));
    pos += (int)sizeof(uint8_t);

    memcpy(&header->version, buf + pos, sizeof(uint8_t));
    pos += (int)sizeof(uint8_t);

    memcpy(&sender_id, buf + pos, sizeof(uint16_t));
    pos += (int)sizeof(uint16_t);
    header->sender_id = ntohs(sender_id);

    memcpy(&payload_len, buf + pos, sizeof(uint16_t));
    pos += (int)sizeof(uint16_t);
    header->payload_len = ntohs(payload_len);
}

void print_header(header_t *header)
{
    printf("HEADER\nPacket Type: %u\nVersion: %u\nSender ID: %u\nPayload Length: %u\n",
           header->packet_type, header->version, header->sender_id, header->payload_len);
}

/* returns # of bytes decoded */
static int decode_field(uint8_t buf[], int pos)
{
    int count = 1;
    uint8_t type = buf[pos];
    switch (type)
    {
        case ASN_INT:
            count += decode_int(buf, pos + 1);
            break;
        case ASN_ENUM:
            count += decode_enum(buf, pos + 1);
            break;
        case ASN_STR:
            count += decode_str(buf, pos + 1);
            break;
        case ASN_TIME:
            count += decode_time(buf, pos + 1);
            break;
        default:
            fprintf(stderr, "Unrecognized packet type: %u\n", type);
    }
    return count;
}

void decode_packet(uint8_t buf[])
{
    header_t header = {0};
    decode_header(buf, &header);
    print_header(&header);
    if (header.payload_len > 0)
    {
        int remaining = header.payload_len;
        int pos = HEADERLEN;
        while(remaining)
        {
            int decoded = decode_field(buf, pos);
            remaining -= decoded;
            pos += decoded;
        }
    }
}
