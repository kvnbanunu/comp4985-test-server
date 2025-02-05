#include "../include/protocol.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>

/* encodes the tag, len, and val of a uint8_t. returns how many bytes were encoded. */
static int encode_uint8(uint8_t buf[], const uint8_t src, const int pos)
{
    int next = pos;
    buf[next++] = PT_INT;
    buf[next++] = 1;
    buf[next++] = src;
    return next - pos;
}

static int encode_seq(uint8_t buf[], const uint8_t len, const int pos)
{
    int next = pos;
    buf[next++] = PT_SEQ;
    buf[next++] = len;
    return next - pos;
}

static int encode_enum(uint8_t buf[], const enum Packet_Type src, const int pos)
{
    int next = pos;
    buf[next++] = PT_ENUM;
    buf[next++] = 1;
    buf[next++] = (uint8_t)src;
    return next - pos;
}

/* encodes the tag, len, and val of a uint16_t. returns how many bytes were encoded. */
static int encode_uint16(uint8_t buf[], uint16_t src, const int pos)
{
    int next = pos;
    uint16_t copy = htons(src);
    buf[next++] = PT_INT;
    buf[next++] = (uint8_t)sizeof(uint16_t);
    memcpy(buf + next, &copy, sizeof(uint16_t));
    return next + (int)sizeof(uint16_t) - pos;
}

static int encode_str(uint8_t buf[], const char *src, const int pos)
{
    int next = pos;
    int len = strlen(src);
    buf[next++] = PT_STR;
    buf[next++] = len;
    memcpy(buf + next, src, len);
    return next + len - pos;
}

static void decode_uint8(const uint8_t buf[], uint8_t *dst, const int pos)
{
    memcpy(dst, buf + pos + 2, 1);
}

static void decode_enum(const uint8_t buf[], enum Packet_Type *dst, const int pos)
{
    uint8_t copy;
    memcpy(&copy, buf + pos + 2, 1);
    *dst = (enum Packet_Type)copy;
}

static void decode_uint16(const uint8_t buf[], uint16_t *dst, const int pos)
{
    uint16_t copy;
    memcpy(&copy, buf + pos + 2, sizeof(uint16_t));
    *dst = ntohs(copy);
}

/* need to free after */
static int decode_str(const uint8_t buf[], char *dst, const int pos)
{
    int len = buf[pos + 1];
    int start = pos + 2;
    dst = (char *)malloc(len + 1);
    memcpy(dst, buf + start, len);
    dst[len] = '\0';
    return 1 + 1 + len;
}

void e_header(uint8_t buf[], const header_t *header)
{
    uint16_t copy;
    memcpy(buf, &header->packet_type, 1);
    memcpy(buf + 1, &header->version, 1);
    copy = htons(header->senderID);
    memcpy(buf + 2, &copy, 2);
    copy = htons(header->payload_len);
    memcpy(buf + 4, &copy, 2);
}

void e000_sys_success(uint8_t buf[], const sys_success_res *res, const uint8_t version)
{
    const int len = 3;
    header_t header = {SYS_SUCCESS, version, SYSID, len};
    uint8_t copy = (uint8_t)res->type;
    e_header(buf, &header);
    encode_uint8(buf, copy, HEADERLEN);
}

void e001_sys_error(uint8_t buf[], const sys_error_res *res, const uint8_t version)
{
    const int len = 3;
    header_t header = {SYS_ERROR, version, SYSID, len};
    uint8_t copy = res->code;
    e_header(buf, &header);
    encode_uint8(buf, copy, HEADERLEN);
}

void e010_acc_login(uint8_t buf[], const acc_login_req *req, const uint8_t version)
{
    // TODO next milestone
}

void e011_acc_login_success(uint8_t buf[], const acc_login_res *res, const uint8_t version)
{
    const int len = 4;
    header_t header = {ACC_LOGIN_SUCCESS, version, SYSID, len};
    uint16_t copy = res->userID;
    e_header(buf, &header);
    encode_uint16(buf, copy, HEADERLEN);
}

void e012_acc_logout(uint8_t buf[], const acc_logout_req *req, const uint8_t version)
{
    // TODO next milestone
}

void e013_acc_create(uint8_t buf[], const acc_create_req *req, const uint8_t version)
{
    // TODO next milestone
}

void e014_acc_edit(uint8_t buf[], const acc_edit_req *req, const uint8_t version)
{
    // TODO next milestone
}

void e020_cht_send(uint8_t buf[], const cht_send_t *msg, const uint8_t version, const uint16_t senderID)
{
    int total;
    int len;
    header_t header = {CHT_SEND, version, senderID, 0};
    total = encode_uint8(buf, msg->groupID, HEADERLEN);
    len = encode_str(buf, msg->content.timestamp, HEADERLEN + total + 2);
    len += encode_str(buf, msg->content.content, HEADERLEN + total + len + 2);
    len += encode_seq(buf, len, HEADERLEN + total);
    header.payload_len = total + len;
    e_header(buf, &header);
}

void e030_lst_get(uint8_t buf[], const lst_get_req *req, const uint8_t version)
{
    // TODO next milestone
}

void e031_lst_response(uint8_t buf[], const lst_res *res, const uint8_t version)
{
    
}

void e040_grp_join(uint8_t buf[], const grp_join_req *req, const uint8_t version);
void e041_grp_exit(uint8_t buf[], const grp_exit_req *req, const uint8_t version);
void e042_grp_create(uint8_t buf[], const grp_create_req *req, const uint8_t version);
void e050_hst_get(uint8_t buf[], const hst_get_req *req, const uint8_t version);

void d_header(const uint8_t buf[], header_t *header)
{
    uint16_t copy;
    int sz = (int)sizeof(uint16_t);
    header->packet_type = buf[0];
    header->version = buf[1];
    memcpy(&copy, buf + sz, sz);
    header->senderID = ntohs(copy);
    memcpy(&copy, buf + sz + sz, sz);
    header->payload_len = ntohs(copy);
}

void d000_sys_success(const uint8_t buf[], sys_success_res *res, const uint8_t version)
{
    // Not needed
}

void d001_sys_error(const uint8_t buf[], sys_error_res *res, const uint8_t version)
{
    // Not needed
}

void d010_acc_login(const uint8_t buf[], acc_login_req *req)
{
    /* NEED TO FREE */
    int pos = decode_str(buf, &req->username, HEADERLEN);
    decode_str(buf, &req->password, pos + HEADERLEN);
}

void d011_acc_login_success(const uint8_t buf[], acc_login_res *res)
{
    // Not needed
}

void d012_acc_logout(const uint8_t buf[], acc_logout_req *req, const uint8_t version)
{
    // Not needed
}

void d013_acc_create(const uint8_t buf[], acc_create_req *req, const uint8_t version)
{
    
}

void d014_acc_edit(const uint8_t buf[], acc_edit_req *req, const uint8_t version);
void d020_cht_send(const uint8_t buf[], cht_send_t *msg, const uint8_t version);
void d030_lst_get(const uint8_t buf[], lst_get_req *req, const uint8_t version);
void d031_lst_response(const uint8_t buf[], lst_res *res, const uint8_t version);
void d040_grp_join(const uint8_t buf[], grp_join_req *req, const uint8_t version);
void d041_grp_exit(const uint8_t buf[], grp_exit_req *req, const uint8_t version);
void d042_grp_create(const uint8_t buf[], grp_create_req *req, const uint8_t version);
void d050_hst_get(const uint8_t buf[], hst_get_req *req, const uint8_t version);
