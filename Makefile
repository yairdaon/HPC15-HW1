EXECUTABLES = first second jaco # sr2 pingpong pingpong_array pingpong_nonblocking
COMPILER = mpicc #mpicc-openmpi-mp
FLAGS = -O3 -Wall

all: $(EXECUTABLES)

first: int_ring.c
	$(COMPILER) $(FLAGS) int_ring.c -o first

second: int_ring.c
	$(COMPILER) $(FLAGS) $^ -o second

run: first
	mpirun -np 7 ./first 25 20000
	
forrest: second
	mpirun -np 4 ./second  1 20



# problem 2
yair: yairbi.c
	$(COMPILER) $(FLAGS) yairbi.c -o yair

yairrun: yair
	mpirun -np 10 ./yair 250 5000

irena: jacobi.c
	$(COMPILER) $(FLAGS) jacobi.c -o irena

irenarun: irena	
	mpirun -np 4 ./irena 1024 500

clean:
	rm -rf $(EXECUTABLES)
	rm -vf *~
