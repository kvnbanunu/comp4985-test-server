#ifndef ASN_H
#define ASN_H

#include <stdint.h>

#define HEADERLEN 6

enum ASNTag
{
    ASN_BOOL = 1,
    ASN_INT = 2,
    ASN_NULL = 5,
    ASN_ENUM = 10,
    ASN_STR = 12,
    ASN_TIME = 24,
    ASN_SEQ = 48
};

typedef struct
{
    uint8_t packet_type;
    uint8_t version;
    uint16_t sender_id;
    uint16_t payload_len;
} header_t;

int decode_int(uint8_t buf[], int pos);
int decode_enum(uint8_t buf[], int pos);
int decode_str(uint8_t buf[], int pos);
int decode_time(uint8_t buf[], int pos);
void decode_header(uint8_t buf[], header_t *header);

#endif // ASN_H
