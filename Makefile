EXECUTABLES = int_ring jacobi-mpi # 
COMPILER = mpicc #mpicc-openmpi-mp
FLAGS = -O3 -Wall

# make all
all: $(EXECUTABLES)
	

# problem 1
int_ring: int_ring.c
	$(COMPILER) $(FLAGS) int_ring.c -o int_ring

runring: int_ring
	mpirun -np 10 ./int_ring 20


# problem 2
jacobi-mpi: jacobi-mpi.c
	$(COMPILER) $(FLAGS) jacobi-mpi.c -o jacobi-mpi

runjacobi: jacobi-mpi
	mpirun -np 2 ./jacobi-mpi 100000000 10

# clean up
clean:
	rm -rf $(EXECUTABLES)
	rm -vf *~
