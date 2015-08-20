CHDRS = defs.h
SRCS = add-voter.c list-candidates.c  vote.c vote-count.c vote-zero.c server-tcp.c 

all: addvoter list vote vcount votezero server

addvoter:
	gcc add-voter.c $(CHDRS) -o add-voter-tcp
list:
	gcc list-candidates.c $(CHDRS) -o list-candidates-tcp

vote:
	gcc vote.c $(CHDRS) -o vote-tcp

vcount:
	gcc vote-count.c $(CHDRS) -o vote-count-tcp

votezero:
	gcc vote-zero.c $(CHDRS) -o vote-zero-tcp

server:
	gcc server-tcp.c server.h $(CHDRS) -o server-tcp

clean:
	rm -rf add-voter-tcp list-candidates-tcp vote-tcp vote-count-tcp vote-zero-tcp server-tcp 
