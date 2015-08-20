
SRCS = add-voter.c list-candidates.c  vote.c vote-count.c vote-zero.c server-udp.c 
CHDRS = defs.h

all: addvoter list vote vcount votezero server

addvoter:
	gcc  add-voter.c $(CHDRS) -o add-voter-udp
list:
	gcc list-candidates.c $(CHDRS) -o list-candidates-udp

vote:
	gcc vote.c $(CHDRS) -o vote-udp

vcount:
	gcc vote-count.c $(CHDRS) -o vote-count-udp

votezero:
	gcc vote-zero.c $(CHDRS) -o vote-zero-udp

server:
	gcc server-udp.c server.h $(CHDRS) -o server-udp

clean:
	rm -rf add-voter-udp list-candidates-udp vote-udp vote-count-udp vote-zero-udp server-udp
