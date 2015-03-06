EXECUTABLES = first second # sr2 pingpong pingpong_array pingpong_nonblocking
COMPILER = mpicc #mpicc-openmpi-mp
FLAGS = -O3 -Wall

all: $(EXECUTABLES)

first: int_ring.c
	$(COMPILER) $(FLAGS) int_ring.c -o first

second: int_ring.c
	$(COMPILER) $(FLAGS) $^ -o second

run: first
	mpirun -np 4 ./first 5
	
forrest: second
	mpirun -np 4 ./second  1
	
clean:
	rm -rf $(EXECUTABLES)
