#include "client.h"

int  tcp_connect(char* ip, char* port) {
	int 				fd;
	struct sockaddr_in	serv_addr;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		error_handling("socket() error", __LINE__);
		
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip);
	serv_addr.sin_port = htons(atoi(port));

	if(connect(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error", __LINE__);
	
	return fd;
}

void send_msg(MSG msg, USR usr, int sockfd, char* bufmsg) {
	char dm[5], change[8];

	strcpy(dm, "/dm"); 
	strcpy(change, "/change");

	/* DM */
	if (bcmp(bufmsg, &dm, 2) == 0) {
		msg.code = 30;
		strcpy(msg.buf, bufmsg);
		write(sockfd, &msg, sizeof(msg));
	}

	/* List */
	else if (!strcmp(bufmsg, "/list\n")) {
		msg.code = 40;
		write(sockfd, &msg, sizeof(msg));
	}

	/* Change name */
	else if (bcmp(bufmsg, &change, 6) == 0) {
		msg.code = 50;
		strcpy(msg.buf, bufmsg);
		write(sockfd, &msg, sizeof(msg));
	}

	/* Chat */
	else {
		msg.code = 20;
		msg.user = usr;
		strcpy(msg.buf, bufmsg);
		write(sockfd, &msg, sizeof(msg));
		console_log(bufmsg, usr.name);
	}
}

void receive_msg(MSG msg, USR *usr, int sockfd, char** argv) {
	char*	buffer = malloc(sizeof(char)*MAXLINE);
	
	ct = time(NULL);		// 현재 시간을 받아옴
	tm = *localtime(&ct);

	switch(msg.code) {

		/*  10. Connect: 서버 접속 성공 메시지 출력  */
		case 10: 
			*usr = msg.user;
			menu(*usr, argv[1], argv[2]);
			fprintf(stderr, "\033[1;33m");			// 글자색을 노란색으로 변경
			strcpy(buffer, msg.buf);
			break;

		/*  11. Connect: Server에 Name 정보 전송  */
		case 11: 				
			strcpy(msg.user.name, argv[3]);
			write(sockfd, &msg, sizeof(msg));
			return;
		
		/*  12. Connect 오류: Name 중복  */
		case 12: 
			system("clear");
			fprintf(stderr, "\033[1;31m");			// 글자색을 빨간색으로 변경
			printf("[%s]는 이미 존재하는 닉네임입니다! 다른 닉네임을 입력해주세요. \n", msg.user.name);
			exit(1);

		/*  13. Connect 오류: 입장가능 인원 초과  */
		case 13: 
			system("clear");
			fprintf(stderr, "\033[1;31m");			// 글자색을 빨간색으로 변경
			printf("입장가능 인원이 초과되어 이용이 불가능합니다. \n");
			exit(1);
					
		/*  20. Chat: 전체 채팅  */
		case 20: 
			fprintf(stderr, "\033[1;37m");			// 글자색을 하얀색으로 변경
			strcpy(buffer, msg.buf);
			break;

		/*  21. Chat: 시스템 알림  */
		/*  40. List  */
		case 21: case 40:
			fprintf(stderr, "\033[1;33m");			// 글자색을 노란색으로 변경
			strcpy(buffer, msg.buf);
			break;

		/*  22. Chat: 서버 공지사항  */
		case 22: 
			fprintf(stderr, "\033[1;35m");			// 글자색을 연보라색으로 변경
			strcpy(buffer, msg.buf);
			break;

		/*  30. DM: DM 수신  */
		case 30: 
			fprintf(stderr, "\033[1;36m");		// 글자색을 Cyan으로 변경
			sprintf(buffer, "[%02d:%02d:%02d] %s >> %s", tm.tm_hour, tm.tm_min, tm.tm_sec, msg.user.name, msg.buf);
			break;

		/*  31. DM: DM 송신  */
		case 31: 
			fprintf(stderr, "\033[1;36m");		// 글자색을 Cyan으로 변경
			fprintf(stderr, "\033[1A"); 		// Y좌표를 현재 위치로부터 -1만큼 이동
			sprintf(buffer, "[%02d:%02d:%02d] %s << %s", tm.tm_hour, tm.tm_min, tm.tm_sec, msg.user.name, msg.buf);
			break;
		
		/*  32. DM 오류: 존재하지 않는 유저  */
		/*  33. DM 오류: 자기 자신에게 전송  */
		/*  51. 닉네임 변경 오류: 닉네임 중복  */
		/*  52. 닉네임 변경 오류: 변경하려는 닉네임이 현재 닉네임과 일치  */
		case 32: case 33: case 51: case 52:
			fprintf(stderr, "\033[1;31m");			// 글자색을 빨간색으로 변경
			strcpy(buffer, msg.buf);
			break;

		/*  50. 닉네임 변경  */
		case 50: 
			fprintf(stderr, "\033[1;33m");		// 글자색을 노란색으로 변경
			strcpy(usr->name, msg.user.name);
			strcpy(buffer, msg.buf);
			break;
	}

	write(1, "\033[0G", 4);					// 커서의 X좌표를 0으로 이동
	printf("%s", buffer);					// 메시지 출력
	free(buffer);

	fprintf(stderr, "\033[1;32m");			// 글자색을 녹색으로 변경
	fprintf(stderr, "%s > ", usr->name);	// 내 닉네임 출력
}

void menu(USR usr, char* ip, char* port) {
	ct = time(NULL);					// 현재 시간을 받아옴
	tm = *localtime(&ct);

    system("clear");
    printf(" *****      CHAT ROOM     *****\n");
    printf(" Server info : %s:%s \n", ip, port);
    printf(" Client info : %s:%d \n", usr.ip, usr.port);
    printf(" Chat name   : %s \n", usr.name);
    printf(" Access time : %02d:%02d:%02d \n", tm.tm_hour, tm.tm_min, tm.tm_sec);
    printf(" \n *****        MENU        *****\n");
    printf(" /list: 현재 접속자 확인\n");
    printf(" /change <NAME>: 이름 변경 \n");
    printf(" /dm <NAME> <내용>: 귓속말 (1:1 대화)\n");
    printf(" \n *****        Log         *****\n");
}

void console_log(char* bufmsg, char* username) {
	char*	buffer = malloc(sizeof(char)*MAXLINE);

	sprintf(buffer, "[%02d:%02d:%02d] %s > %s", tm.tm_hour, tm.tm_min, tm.tm_sec, username, bufmsg);
	fprintf(stderr, "\033[1A"); 			// Y좌표를 현재 위치로부터 -1만큼 이동
	fprintf(stderr, "\033[1;37m");			// 글자색을 하얀색으로 변경
	write(1, "\033[0G", 4);					// 커서의 X좌표를 0으로 이동
	printf("%s", buffer);					// 메시지 출력

	fprintf(stderr, "\033[1;32m");			// 글자색을 녹색으로 변경
	fprintf(stderr, "%s > ", username);		// 내 닉네임 출력

	free(buffer);
}

void error_handling(char *msg, int line) {
	fputs(msg, stderr);
    printf("Error occured at line %d \n", line);
	fputc('\n', stderr);
	exit(1);
}