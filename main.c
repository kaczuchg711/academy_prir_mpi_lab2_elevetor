#include<stdio.h>
#include<mpi/mpi.h>
#include<string.h>
#include <stdlib.h>
#include <time.h>


#define NUMBER_OF_FLOORS 12
#define SIZE 3


int rand_int(int min_num, int max_num) {
    int result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num) {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    } else {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }

    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}


typedef struct {
    int process_localization[SIZE];
    int direction[SIZE];
    int process_end_floor[SIZE];
    int actual_floor;

} Elevator;


void call_elevator(int rank, int start_floor, int direction,int end_floor, Elevator elevator) {
//elevetor get information where is process and in which direction want go
//  0 - down, 1 - up
    elevator.process_localization[rank] = start_floor;
    elevator.direction[rank] = direction;
    elevator.process_end_floor[rank] = end_floor;

}


int main(int argc, char **argv) {
    srand(time(NULL));

    int rank, size, r;
    int src, dst, tag, i;
    MPI_Status status;

    MPI_Datatype new_type;
    MPI_Datatype type[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    int blen[4] = {SIZE, SIZE,SIZE, 1};
    MPI_Aint disp[4];
    MPI_Aint base, addr;
    Elevator elevator;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != SIZE) {
        printf("set proper SIZE");
        exit(0);
    }
//    creating structure
    if (rank == 0)
        printf("MPI_Type_create_struct()\n");

    MPI_Get_address(&elevator, &base);
    MPI_Get_address(&(elevator.process_localization), &addr);
    disp[0] = addr - base;
    MPI_Get_address(&(elevator.direction), &addr);
    disp[1] = addr - base;
    MPI_Get_address(&(elevator.process_end_floor), &addr);
    disp[2] = addr - base;
    MPI_Get_address(&(elevator.actual_floor), &addr);
    disp[3] = addr - base;

    MPI_Type_create_struct(4, blen, disp, type, &new_type);
    MPI_Type_commit(&new_type);

//    my code
    int start_floor;
    int end_floor;


    if (rank == 0) {

        for (int j = 0; j < size - 1; ++j) {
            start_floor = rand_int(0, NUMBER_OF_FLOORS);
            end_floor = rand_int(0, NUMBER_OF_FLOORS);
            MPI_Send(&start_floor, 1, MPI_INT, j + 1, tag, MPI_COMM_WORLD);
            MPI_Send(&end_floor, 1, MPI_INT, j + 1, tag, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(&start_floor, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&end_floor, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        printf("I am process %d and I start on  %d floor and I want go on %d\n", rank, start_floor, end_floor);
        int direction = end_floor > start_floor ? 1 : 0;
        int j = 0;
        while (rank < size) {
            if (j == rank) {
                call_elevator(rank, start_floor, direction,end_floor, elevator);
//          todo send info abaout elevetor
            }
            j ++;
            MPI_Barrier(MPI_COMM_WORLD);
            printf("Every process call Elevator");
        }

    }
    //MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}
