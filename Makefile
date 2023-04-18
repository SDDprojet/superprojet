GCC_FLAGS=-Wall -g 
EXEC= myGit 

all: $(EXEC)

clean:
	rm -f *.o $(EXEC) *~

.c.o:   
	gcc $(GCC_FLAGS) -c $*.c

myGit: myGit.o liste.o commit.o filehash.o worktf.o ref.o branch.o checkout.o merge.o
	gcc ${GCC_FLAGS} -o $@ $^

main : main.o liste.o commit.o filehash.o worktf.o ref.o branch.o checkout.o merge.o
	gcc ${GCC_FLAGS} -o $@ $^