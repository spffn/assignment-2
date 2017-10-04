CC=gcc
ODIR=obj

all: master child

master: test.c
	$(CC) -g -o master test.c
	
child: child.c
	$(CC) -g -o palin child.c

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~