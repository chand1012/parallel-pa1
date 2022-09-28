
PROCESSES := env_var_or_default("PROCESSES", "2")

clean:
    rm -f a b c test
    rm -rf PA1

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
    if [ -z $(which mpicc) ]; then echo "mpicc not found in path"; exit 1; else echo "mpicc good"; fi
    if [ -z $(which mpirun) ]; then echo "mpirun not found in path"; exit 1; else echo "mpirun good"; fi

zip:
    mkdir -p PA1
    cp pa1a.c pa1b.c pa1c.c PA1
    cp performance.txt PA1
    zip -r PA1.zip PA1
    
build: build-a build-b build-c

run: run-a run-b run-c

default: doctor build run zip clean
