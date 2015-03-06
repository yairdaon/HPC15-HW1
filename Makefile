EXECUTABLES = first second jaco # sr2 pingpong pingpong_array pingpong_nonblocking
COMPILER = mpicc #mpicc-openmpi-mp
FLAGS = -O3 -Wall

all: $(EXECUTABLES)

first: int_ring.c
	$(COMPILER) $(FLAGS) int_ring.c -o first

second: int_ring.c
	$(COMPILER) $(FLAGS) $^ -o second

jaco: jacobi.c
	$(COMPILER) $(FLAGS) jacobi.c -o jaco

run: first
	mpirun -np 7 ./first 25 200000000
	
forrest: second
	mpirun -np 4 ./second  1 20




jacorun: jaco
	mpirun -np 4 ./jaco 9 3



clean:
	rm -rf $(EXECUTABLES)
