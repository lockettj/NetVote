//ADD VOTER CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "defs.h"

int main (int argc, char** argv){

	if (argc != 5){
		fprintf(stderr, "USAGE: %s <ip address> <port> <name> <id>\n", argv[0]);
		return 1;
	}

	char buf[2048];
	int sockfd;
	struct sockaddr_in servaddr;
	struct sockaddr_in myaddr;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("cannot create socket");
		return 1;
	}

	memset((char*)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);
	
	if (bind(sockfd, (struct sockaddr *)&myaddr, sizeof(myaddr))< 0){
		perror("bind failed");
		return 1;
	}

	memset((char*)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	
	 if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <=0){
                perror("Error with inet_pton");
                return 2;
        }
	

	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))< 0){
		perror("connect failed");
		return 1;
	}

	
      	
	sprintf(buf, "2 %s %d", argv[3], atoi(argv[4]));
	write(sockfd, buf, strlen(buf)); 
	
	int by = read(sockfd, buf, 1024);
	buf[by] = '\0';
	int c = buf[0] - '0';
	switch(c){
		case NEW:
			printf("\n%s\n", STRING_NEW);
			break;
		case EXISTS:
			printf("\n%s\n", STRING_EXISTS);
			break;
		case ERROR:
			printf("\n%s\n", STRING_ERROR);
			break;
		case NOTAVOTER:
			printf("\n%s\n", STRING_NOTAVOTER);
			break;
		case ALREADYVOTED:
			printf("\n%s\n", STRING_ALREADYVOTED);
			break;
	}

	//printf("RESULT: %s\n", buf);

}


