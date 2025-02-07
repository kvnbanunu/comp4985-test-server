#ifndef ASN_H
#define ASN_H

#include <stdint.h>
#include <system_error>

#define HEADERLEN 6
#define TIMESTRLEN 15
#define PACKETLEN 771
#define SYSID 0
#define CURRVER 1
#define SYS_SUCCESS_LEN 3
#define ACC_LOGIN_SUCCESS_LEN 4

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

enum Packet_Type
{
    SYS_SUCCESS,
    SYS_ERROR,
    ACC_LOGIN = 10,
    ACC_LOGIN_SUCCESS,
    ACC_LOGOUT,
    ACC_CREATE,
    ACC_EDIT,
    CHT_SEND = 20,
    LST_GET = 30,
    LST_RESPONSE,
    GRP_JOIN = 40,
    GRP_EXIT,
    GRP_CREATE,
    HST_GET = 50
};

enum Error_Code
{
    EC_INVUSERID = 11,
    EC_INVAUTHINFO,
    EC_USEREXISTS,
    EC_GENSERVER = 21,
    EC_INVREQ = 31,
    EC_REQTO
};

typedef struct header_t
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
int check_header(header_t *header);
void print_header(header_t *header);
int decode_packet(uint8_t buf[], header_t *header);
int encode_sys_success_res(uint8_t buf[], uint8_t packet_type);
int encode_sys_error_res(uint8_t buf[], uint8_t err);
int encode_acc_login_success_res(uint8_t buf[], uint16_t user_id);

#endif // ASN_H
