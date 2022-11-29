mpicc main.c -o main
mpirun -np 3 --display-map ./main
