#ifndef HEADER
#define HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>

#define MAXNAME 20
#define MAXLINE 4096

time_t			ct;
struct 	tm		tm;

typedef struct user {
	char		name[MAXNAME];
	char		ip[16];
	int			port;
	int			fd;
} USR;

typedef struct message {
	int 		code;
	char		buf[MAXLINE];
	USR			user;
} MSG;

int  			tcp_connect(char* ip, char* port);
void 			send_msg(MSG msg, USR usr, int sockfd, char* bufmsg);
void 			receive_msg(MSG msg, USR *usr, int code, char** argv);
void 			menu(USR usr, char* ip, char* port);
void 			console_log(char* bufmsg, char* username);
void 			error_handling(char* msg, int line);

#endif