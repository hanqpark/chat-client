#include "client.h"

int main(int argc, char **argv) {
	int		maxfdp1, nbyte, sockfd;
	char	bufmsg[MAXLINE];
	fd_set	reads;
	MSG		msg;
	USR		usr;

	if (argc != 4) {
		printf("Usage: %s <IP> <PORT> <NAME>\n", argv[0]);
		exit(1);
	}

	sockfd = tcp_connect(argv[1], argv[2]);
				
	FD_ZERO(&reads);
	while(1) {
		FD_SET(0, &reads);
		FD_SET(sockfd, &reads);
        bzero(&msg, sizeof(msg));
		msg.user = usr;
		if (select(sockfd+1, &reads, NULL, NULL, NULL) < 0)
			error_handling("select() error", __LINE__);

		/* from network */
		if (FD_ISSET(sockfd, &reads)) {	
			nbyte = read(sockfd, &msg, sizeof(msg));
			if (nbyte == 0) {
				printf("서버가 종료되었습니다.");
				exit(1);
			} else
				receive_msg(msg, &usr, sockfd, argv);		
		}			

		/* from me */
		if (FD_ISSET(0, &reads)) {
			bzero(&bufmsg, sizeof(bufmsg));
			nbyte = read(0, bufmsg, sizeof(bufmsg));
			send_msg(msg, usr, sockfd, bufmsg);
		}
	}
	exit(0);
}
