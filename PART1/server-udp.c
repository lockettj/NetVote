#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "server.h"
#include "defs.h"

#define DEBUG 1

#define PORT 10293
#define BUFSIZE 2048
#define MAX_CANDIDATES 32
#define NUM_VOTERS 32

//TODO free allocated memory

int num_candidates = 0;
int num_voters = 0;

struct Candidate *candidates[MAX_CANDIDATES];
struct Voter *voters [NUM_VOTERS];

void test(unsigned char buf []);
void add_candidate(char*cand_name){
	//
	if (DEBUG) printf("--- ADD CANDIDATE CALLED\n");
	if (num_candidates < MAX_CANDIDATES){
		struct Candidate * cd = malloc(sizeof(struct Candidate));
		cd->name = strdup(cand_name);
		cd->votecount = 1;
		candidates[num_candidates] = cd;
		
		num_candidates +=1;
	}
	else{//max number of candidates reached, either resize or fail

	}
}
void voter_print(struct Voter *v){
	printf("%d: %s", v->id, v->voted?"VOTED\n":"HAS NOT VOTED\n");

}

void candidate_print(struct Candidate *c){
	printf("%s, %d votes\n", c->name, c->votecount);
}



int main (){
	//declare adresses
	struct sockaddr_in addr; /*This server address*/
	struct sockaddr_in remaddr;   /*a remote address*/
	
	socklen_t addrlen = sizeof(remaddr); //length of adresses
	int recvlen;				//# bytes received
	int recv_socket;			// this server's socket
	unsigned char buf [BUFSIZE];		//receive buffer

	/*Create the UDP socket*/
	if ((recv_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("cannot create socket\n");
		return 0;
	}

	memset((char*)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);

	if (bind(recv_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("bind to socket failed");
		return 0;
	}

	char c;
	/*Now the server should loop, receiving data*/
	while(1){
		printf("waiting on port %d\n", PORT);
		recvlen = recvfrom(recv_socket, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		printf("received %d bytes \n", recvlen);
		
		if (recvlen > 0){
			buf[recvlen] = 0;
			test(buf);
			
			c = buf[0];
			char *token;
			
			//put call to service method here
			switch (c){
			case ZEROIZE:
				printf("Zeroize\n");
				
				int z = zeroize();
				//return result
				sprintf(buf, "%d\n", z);
				break;
			case ADDVOTER:
				printf("Add Voter\n");
				
				//validate input
				
				//tokenize
				token = strtok(buf, " ");
				token = strtok(NULL, " ");//id will be the second token
	
				int result = add_voter(atoi(token)); //should check that this doesn't fail
				if (DEBUG) list_voters();
				//return the result to client

				sprintf(buf, "%d", result);
				break;
			case VOTE:
				printf("Vote\n");
				token = strtok(buf, " ");
				//validate input
				token = strtok(NULL, " "); //candidate name is second 
				int a = vote_for(token, atoi(strtok(NULL, " "))); //TODO validate
				printf("  CODE: %d\n", a);
				sprintf(buf, "%d\n", a);
				break;
			case LISTCANDIDATES:
				printf("List Candidates\n");
				list_candidates((unsigned char *) &buf);
				printf("NEW BUFFER %s %d\n", buf, strlen(buf));
				break;
			case VOTECOUNT:
				token = strtok(buf, " ");
				token = strtok(NULL, " ");
			
				int count = vote_count(token);
				printf("Votes %s : %d\n", token, count);

				//return this vote count to client
				sprintf(buf, "%s : %d", token, count);
				break;
			default:
				printf("Do Default Thing\n");
			
			}

			if(DEBUG)printf("Sending %s length:%d\n", buf, strlen(buf));	
			sendto(recv_socket, buf, strlen(buf),0,  (struct sockaddr *)&remaddr,addrlen ); 
		}

	}	
			printf("vote-zero\n");
	return 0;
}

int  zeroize(){
	num_voters = 0;
	num_candidates = 0;
	
	return 1;	
}

void test(unsigned char buf [] ){
	printf("The Received Message: \"%s\"\n", buf);
}

int registered_voter(int id){
	int i;
	for (i = 0; i<num_voters; ++i ){
		if(voters[i]->id == id){
			return 1;
		}
	}
	return 0;	
}

int add_voter(int id){
	if (registered_voter(id)){
		if (DEBUG)printf("Voter Exists\n");
		return EXISTS;
	}
	else{
		//create voter
		struct Voter *vot = malloc(sizeof(struct Voter));
		vot ->id = id;
		vot ->voted =	0;

		voters[num_voters] = vot;
		num_voters++;
		if (DEBUG) printf("Voter Added\n");
		return OK;
	}
	
}

int  vote_for(char* name, int id){
	if (DEBUG) printf("IN VOTE_FOR\n\n");
	//check if id is registered voter
	if (!(registered_voter(id))){
		if (DEBUG) printf("NOT A VOTER\n");
		return NOTAVOTER;
	}
	
	int i;
	//check if voted already
	for (i = 0; i<num_voters; ++i){
		if (voters[i]->id == id)break;
	}
	
	if (voters[i]->voted) return ALREADYVOTED;
	//set voted flag
	voters[i]->voted = 1;

	//if (DEBUG) printf("VALIDATED VOTER\n\n");
	//check if name is a candidate
	for (i = 0; i< num_candidates; ++i){
		if (DEBUG)printf("Checking %d %s : %s", i, name, candidates[i]->name);
		if (strcmp(name, candidates[i]->name) == 0){
			candidates[i]->votecount +=1;
			return EXISTS;
		}
	}
	add_candidate(name);
 
	if (DEBUG) printf("NUM CANDIDATES %d\n", num_candidates);
	return NEW;

}

void list_candidates(unsigned char * buf){
	//print out candidates for now
	char * start = buf;
	int i;
	for (i = 0; i<num_candidates; ++i){
		buf = buf + sprintf(buf, "%s ", candidates[i]->name);
		candidate_print(candidates[i]);
	}
	if (num_candidates == 0){
		sprintf(buf, "NO CANDIDATES");
	}
	buf = start;
	//sprintf(buf, "%s", )

}

int registered_candidate(char *name){
	int i;
	for (i = 0; i< num_candidates; ++i){
		if (strcmp(name, candidates[i]->name) == 0){
			return 1;
		}
	}
	printf("NO CANDIDATE %s\n", name);
	return 0;
	
}

int vote_count(char *name){
	int i;
	//check if name is a candidate
	if (!(registered_candidate(name))){
		return -1;
	}
	
	for (i = 0; i< num_candidates; ++i){
		if (strcmp(name, candidates[i]->name) == 0){
			return candidates[i]->votecount;
		}
	}
}

//not necessary
void list_voters(){
	//
	int i;
	for (i = 0; i<num_voters; ++i){
		voter_print(voters[i]);
	}
}
