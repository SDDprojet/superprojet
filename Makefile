GCC_FLAGS=-Wall -g 
EXEC= main 

all: $(EXEC)

clean:
	rm -f *.o $(EXEC) *~

.c.o:   
	gcc $(GCC_FLAGS) -c $*.c

main: main.o liste.o commit.o filehash.o worktf.o ref.o branch.o
	gcc ${GCC_FLAGS} -o $@ $^