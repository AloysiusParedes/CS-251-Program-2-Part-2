all: sq_slow.o sq.o llist.o driver fdriver

ll_tst: llist.o ll_tst.c
	gcc ll_tst.c llist.o -o ll_tst

llist.o:  list.h llist.c
	gcc -c llist.c

sq_slow.o: llist.o sq.h sq_slow.c
	gcc -c sq_slow.c
	
sq.o: llist.o sq.h sq.c
	gcc -c sq.c

driver: driver.c sq_slow.o 
	gcc driver.c sq_slow.o llist.o -lc -o driver

fdriver: driver.c sq.o 
	gcc fdriver.c sq.o llist.o -lc -o fdriver

