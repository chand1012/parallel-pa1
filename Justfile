
PROCESSES := env_var_or_default("PROCESSES", "2")

clean:
    rm -f a b c test

build-a:
    mpicc -o a pa1a.c

run-a: 
    if [ -f a ]; then mpirun -n 3 ./a; else just build-a; mpirun -n 3 ./a; fi

build-b:
    mpicc -o b pa1b.c

a: run-a clean

run-b: 
    if [ -f b ]; then mpirun -n 2 ./b; else just build-b; mpirun -n 2 ./b; fi

build-c:
    mpicc -o c pa1c.c

b: run-b clean

run-c: 
    if [ -f c ]; then mpirun -n {{PROCESSES}} ./c; else just build-c; mpirun -n {{PROCESSES}} ./c; fi

c: run-c clean

build-test:
    mpicc -o test test.c

run-test:
    if [ -f test ]; then ./test; else just build-test; ./test; fi

test: run-test clean

# check if mpi is install by making sure mpicc is in the path
@doctor:
    if [ -z $(which mpicc) ]; then echo "mpicc not found in path"; else echo "mpicc good"; fi
    if [ -z $(which mpirun) ]; then echo "mpirun not found in path"; else echo "mpirun good"; fi

build: build-a build-b build-c

run: run-a run-b run-c

default: build run clean
