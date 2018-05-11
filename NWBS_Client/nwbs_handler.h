#ifndef __NWBS_HANDLER_H__
#define __NWBS_HANDLER_H__

#include "nwbs_types.h"
#include "nwbs_errno.h"

void guesthelp_handler();
void adminhelp_handler();
void userhelp_handler();
void exit_handler();
int  signup_handler(int sockfd, nwbs_user_t user);
int  signin_handler(int sockfd, nwbs_user_t *user);
int  signout_handler(nwbs_curuser_t *curuser);
int chpswd_handler(int sockfd, nwbs_chpswd_t chpswd);

#endif