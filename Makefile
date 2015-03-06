EXECUTABLES = first second # sr2 pingpong pingpong_array pingpong_nonblocking
COMPILER = mpicc #mpicc-openmpi-mp
FLAGS = -O3 -Wall

all: $(EXECUTABLES)

first: int_ring.c
	$(COMPILER) $(FLAGS) int_ring.c -o first

second: int_ring.c
	$(COMPILER) $(FLAGS) $^ -o second

run: first
	mpirun -np 3 ./first 10		
	
forrest: second
	mpirun -np 3 ./second  10
	
clean:
	rm -rf $(EXECUTABLES)
