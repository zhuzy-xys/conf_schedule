#main

#PARAM
CC     = g++ 
CFLAGS = -g -c -Wall -I/usr/local/include/hconf
OBJS   = demo.o

#.PHONY
.PHONY : all build clean

all : build clean

build : $(OBJS)
	$(CC) -o $@ $^ -lhconf

clean : 
	rm -fr $(OBJS)

#OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
