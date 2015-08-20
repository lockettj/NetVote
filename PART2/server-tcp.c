#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include "server.h"
#include "defs.h"
#include <sys/errno.h>

#ifndef ERESTART
#define ERESTART EINTR
#endif
extern int errno;

void disconn(void);
//----------------------------------------------------
#define DEBUG 0
#define BUFFERSIZE 2048
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

//--------------------------------------------------------------------------
int main (int argc, char **argv){
	int sock; //socket accepting request
	int rqst;
	
	socklen_t alen;	//length of address structure
	struct sockaddr_in addr; //address of this server
	struct sockaddr_in client_addr;	//client's address
	
	int sockoptval = 1;
	char hostname[128];
	char buf[1024];
	gethostname(hostname, 128);
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("cannot create socket");	
		return 1;
	}



	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));


	memset((char*)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	

	if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("bind failed");
		return 1;
	}
	
	if (listen(sock, 5) < 0){
		perror("Listen failed");
		return 1;
	}


	printf("Server started on %s, listening on port %d\n", hostname, PORT);
	
	alen = sizeof(client_addr);
	char c;
	char *token;
	for (;;){
		printf(".\n");	//for some reason this is necessary to get connections
		while((rqst = accept(sock, (struct sockaddr *)&client_addr, &alen)) < 0){
		  if ((errno != ECHILD) && (errno != ERESTART) && (errno != EINTR)){
		    perror("accep failed");
		    exit(1);
		  }
		}
		printf("received a connection from: %s port %d", 
		       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		int nbytes = read(rqst, buf, 1024);
		//make sure we got everything
		buf[nbytes] = '\0';	
		printf("RECEIVED: %s\n", buf);
		//SWITCH ON RECEIVED COMMAND
		c = buf[0];	

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
                                int a = vote_for(token, atoi(strtok(NULL, " ")));
//TODO validate
                                printf("  CODE: %d\n", a);
                                sprintf(buf, "%d\n", a);
                                break;
                        case LISTCANDIDATES:
                                printf("List Candidates\n");
                                list_candidates((unsigned char *)&buf);
                                printf("NEW BUFFER %s %d\n", buf, strlen(buf));
                                break;
                        case VOTECOUNT:
                                token = strtok(buf, " ");
                                token = strtok(NULL, " ");

                                int count = vote_count(token);
                                printf("VOTES %s : %d\n", token, count);

                                //return this vote count to client
                                sprintf(buf, "%s : %d", token, count);
                                break;
                        default:
                                printf("Do Default Thing\n");

                        }
			printf("\nNEW BUFFER:\n%s\n", buf);
			write(rqst, buf, strlen(buf));
	}
}

//FUNCTIONS-------------------------------------

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
                vot ->voted =   0;

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
		sprintf(buf, "No Candidates");
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
