GCC_FLAGS=-Wall -g 
EXEC= main 

all: $(EXEC)

clean:
	rm -f *.o $(EXEC) *~

.c.o:   
	gcc $(GCC_FLAGS) -c $*.c

main: main_exo7.o liste.o commit.o filehash.o worktf.o ref.o
	gcc ${GCC_FLAGS} -o $@ $^