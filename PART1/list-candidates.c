//List Candidates CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "defs.h"
int main (int argc, char* argv[]){
	char buf[256];
		
	//create socket to connect to 
	int sockfd;
	
	struct sockaddr_in servaddr;
	socklen_t addrlen = sizeof (struct sockaddr_in);
	if (argc != 3){
		fprintf(stderr, "Usage %s <ip> <port>\n", argv[0]);
		
		return 1;
	}
	
	
	if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0 )) < 0 ) //change to UDP socket
	{
		perror("error  openning socket" );

    		return 3;
  	}

	memset((char*)&servaddr, 0, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));

	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <=0){
		perror("Error with inet_pton");
		return 2;
	}
	
	
		
	sprintf(buf, "3");
	printf("The Sent Buffer: %s\n", buf);
	if(sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1){
		perror("WRITE ERROR\n");
	}
	
	//get response code
	int x = recvfrom(sockfd, buf, 2048, 0, (struct sockaddr *)&servaddr, &addrlen); 
	if(x == -1){
		perror("READ ERROR\n");
	}
	else{
		buf[x] = '\0';
		printf("\nCandidates\n------------------\n%s\n", buf);
	}
	close(sockfd);
	return 0;
}
