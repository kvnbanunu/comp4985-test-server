#include "../include/protocol.h"
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>

static int encode_uint8(uint8_t buf[], const uint8_t src, const int pos)
{
    buf[pos] = src;
    return 1;
}

static int encode_uint16(uint8_t buf[], uint16_t src, const int pos)
{
    uint16_t copy = htons(src);
    memcpy(buf + pos, &copy, sizeof(uint16_t));
    return 2;
}

static int encode_str(uint8_t buf[], const char *src, const int pos)
{
    memcpy(buf + pos, src, len);
}

static void decode_uint8(const uint8_t buf[], uint8_t *dst, const int pos)
{
    memcpy(dst, buf + pos, 1);
}

static void decode_uint16(const uint8_t buf[], uint16_t *dst, const int pos)
{
    uint16_t copy;
    memcpy(&copy, buf + pos, sizeof(uint16_t));
    *dst = ntohs(copy);
}

static void decode_str(const uint8_t buf[], uint8_t *dst, const int pos, const int len)
{
    memcpy(dst, buf + pos, len);
}

int e_header(uint8_t buf[], const header_t *header);
int e000_sys_success(uint8_t buf[], const sys_success_res *res);
int e001_sys_error(uint8_t buf[], const sys_error_res *res);
int e010_acc_login(uint8_t buf[], const acc_login_req *req);
int e011_acc_login_success(uint8_t buf[], const acc_login_res *res);
int e012_acc_logout(uint8_t buf[], const acc_logout_req *req);
int e013_acc_create(uint8_t buf[], const acc_create_req *req);
int e014_acc_edit(uint8_t buf[], const acc_edit_req *req);
int e020_cht_send(uint8_t buf[], const cht_send_t *msg);
int e030_lst_get(uint8_t buf[], const lst_get_req *req);
int e031_lst_response(uint8_t buf[], const lst_res *res);
int e040_grp_join(uint8_t buf[], const grp_join_req *req);
int e041_grp_exit(uint8_t buf[], const grp_exit_req *req);
int e042_grp_create(uint8_t buf[], const grp_create_req *req);
int e050_hst_get(uint8_t buf[], const hst_get_req *req);
int e_packet(uint8_t buf[P_BUFMAX], const uint8_t header[], const uint8_t payload[]);
int d_header(const uint8_t buf[], header_t *header);
int d000_sys_success(const uint8_t buf[], sys_success_res *res);
int d001_sys_error(const uint8_t buf[], sys_error_res *res);
int d010_acc_login(const uint8_t buf[], acc_login_req *req);
int d011_acc_login_success(const uint8_t buf[], acc_login_res *res);
int d012_acc_logout(const uint8_t buf[], acc_logout_req *req);
int d013_acc_create(const uint8_t buf[], acc_create_req *req);
int d014_acc_edit(const uint8_t buf[], acc_edit_req *req);
int d020_cht_send(const uint8_t buf[], cht_send_t *msg);
int d030_lst_get(const uint8_t buf[], lst_get_req *req);
int d031_lst_response(const uint8_t buf[], lst_res *res);
int d040_grp_join(const uint8_t buf[], grp_join_req *req);
int d041_grp_exit(const uint8_t buf[], grp_exit_req *req);
int d042_grp_create(const uint8_t buf[], grp_create_req *req);
int d050_hst_get(const uint8_t buf[], hst_get_req *req);
int d_packet(const uint8_t buf[P_BUFMAX], uint8_t header[], uint8_t payload[]);
