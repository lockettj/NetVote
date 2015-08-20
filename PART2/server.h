#define PORT 10293 //last 5 digits of student ID

//client actions 
#define ZEROIZE	'0'
#define ADDVOTER	'1'
#define VOTE		'2'
#define LISTCANDIDATES	'3'
#define VOTECOUNT	'4'

//data structures
struct Voter{
	int id;	//The voter's unique id
	int voted; //Boolean, whether or not this user voted
};

struct Candidate{
	int exists; //Whether or not this candidate exists in the system
	char * name; //This candidate's name, unique
	int votecount; //This candidate's vote count

};

int  add_voter(int id);
void list_voters();	//print the list of voters
void list_candidates(unsigned char * buf);	//print the list of candidates
int  vote_for(char *name, int id);	//vote for a candidate
int vote_count(char *name);
int  zeroize();




