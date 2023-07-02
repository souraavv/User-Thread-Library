CC=gcc
CFLAGS=-Wall


SDIR=./src
ODIR=./obj
OBJ0=./obj/queue.o ./obj/utility.o ./obj/userthread.o ./obj/mysemaphore.o 
OBJ1=./obj/test1.o
OBJ2=./obj/test2.o
OBJ3=./obj/test3.o
OBJ4=./obj/test4.o
OBJ5=./obj/test5.o
OBJ6=./obj/test6.o
OBJ7=./obj/test7.o
OBJ8=./obj/boundedBuffer.o
OBJ9=./obj/matrix.o

all: $(OBJ0)
	$(CC) -shared -o ./lib/libMyOwnThread.so ./obj/userthread.o ./obj/mysemaphore.o ./obj/queue.o ./obj/utility.o

plot:
	gnuplot src/script.p

setexport:
	export LD_LIBRARY_PATH=/home/sourav/IITDELHI/Sem1/SSL_COL_701/Assignment3/lib:$LD_LIBRARY_PATH


test1: $(OBJ1)
	$(CC) -L./lib $(CFLAGS) -o test1 ./src/test1.c -lMyOwnThread

$(ODIR)/%.o: $(SDIR)/%.c
	gcc -c -Wall -Werror -fpic -I$(SDIR) $< -o $@
	

test2: $(OBJ2)
	$(CC) -L./lib $(CFLAGS) -o test2 ./src/test2.c -lMyOwnThread

test3: $(OBJ3)
	$(CC) -L./lib $(CFLAGS) -o test3 ./src/test3.c -lMyOwnThread

test4: $(OBJ4)
	$(CC) -L./lib $(CFLAGS) -o test4 ./src/test4.c -lMyOwnThread

test5: $(OBJ5)
	$(CC) -L./lib $(CFLAGS) -o test5 ./src/test5.c -lMyOwnThread

test6: $(OBJ6)
	$(CC) -L./lib $(CFLAGS) -o test6 ./src/test6.c -lMyOwnThread

test7: $(OBJ7)
	$(CC) -L./lib $(CFLAGS) -o test7 ./src/test7.c -lMyOwnThread

boundedBuffer: $(OBJ8)
	$(CC) -L./lib $(CFLAGS) -o boundedBuffer ./src/boundedBuffer.c -lMyOwnThread

matrix: $(OBJ9)
	$(CC) -L./lib $(CFLAGS) -o matrix ./src/matrix.c -lMyOwnThread -lpthread

runtest1: 
	./test1

runtest2: 
	./test2

runtest3: 
	./test3

runtest4: 
	./test4

runtest5: 
	./test5

runtest6: 
	./test6

runtest7: 
	./test7

run_bb: 
	./boundedBuffer $(n) $(m)

run_mm: 
	./matrix $(n) $(tc)

clean:
	rm -rf  $(ODIR)/*.o ./lib/libMyOwnThread.so boundedBuffer matrix all ./test4 ./test5 ./test6 ./test7 ./test3 ./test2 ./test1 mat.dat mat2.dat
	
