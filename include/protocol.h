#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define P_BUFMAX 65541 /* 2 bytes - 1 + header size (6 bytes) */
#define TIMESTAMP_LEN 15 /* YYYYMMDDhhmmssZ */

enum ProtocolTag
{
    PT_BOOL = 1,
    PT_INT = 2,
    PT_NULL = 5
};

typedef struct ProtocolHeader
{
    uint8_t packet_type;
    uint8_t version;
    uint16_t senderID;
    uint16_t packet_len;
} header_t;

typedef struct ChatMessage
{
    uint8_t timestamp[TIMESTAMP_LEN];
    uint8_t *content;
} chat_message_t;

enum UserStatus
{
    US_OFFLINE,
    US_ONLINE,
    US_BUSY
};

typedef struct User
{
    uint32_t id;
    uint8_t *name;
    enum UserStatus status;
} user_t;

typedef struct UserList
{
user_t users[];
} user_list_t;

typedef struct ProtocolError
{
    uint8_t code;
    uint8_t *message;
} error_t;

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

typedef struct SYS_Success
{
    enum Packet_Type type;
} sys_success_res;

typedef struct SYS_Error
{
    uint8_t code;
} sys_error_res;

typedef struct ACC_Login
{
    uint8_t *username;
    uint8_t *password;
} acc_login_req;

typedef struct ACC_Logout acc_logout_req; /* no fields */

typedef struct ACC_Login_Success
{
    uint16_t userID;
} acc_login_res;

typedef struct ACC_Create
{
    uint8_t *username;
    uint8_t *password;
} acc_create_req;

enum ACC_Edit_Type
{
    AET_USERNAME,
    AET_PASSWORD
};

typedef struct ACC_Edit
{
    enum ACC_Edit_Type field;
    uint8_t *value;
} acc_edit_req;

typedef struct CHT_Send
{
    uint8_t groupID;
    chat_message_t content;
} cht_send_t;

enum UserList_Filter
{
    ULF_NONE,
    ULF_ONLINE
};

typedef struct LST_Get
{
    uint8_t groupID;
    enum UserList_Filter filter;
} lst_get_req;

typedef struct LST_Response
{
    user_list_t list;
} lst_res;

/* Placeholder structs -----START */

typedef struct GRP_Join
{
    uint8_t placeholder;
} grp_join_req;

typedef struct GRP_Exit
{
    uint8_t placeholder;
} grp_exit_req;

typedef struct GRP_Create
{
    uint8_t placeholder;
} grp_create_req;

typedef struct HST_Get
{
    uint8_t placeholder;
} hst_get_req;

/* Placeholder structs -----END */

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

#endif // !DEBUG
