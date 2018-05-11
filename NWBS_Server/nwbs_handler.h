#ifndef __NWBS_HANDLER_H__
#define __NWBS_HANDLER_H__

#include "nwbs_types.h"
#include "nwbs_errno.h"

int msg_handler(int sockfd, nwbs_proto_t cli_msg);
int success_handler(int sockfd, nwbs_proto_t cli_msg);
int signup_handler(int sockfd, nwbs_proto_t cli_msg);
int signin_handler(int sockfd, nwbs_proto_t cli_msg);
int chpswd_handler(int sockfd, nwbs_proto_t cli_msg);
int exit_handler();
int sql_init(MYSQL *sql_db);

#endif