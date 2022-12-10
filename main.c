#include<stdio.h>
#include<mpi/mpi.h>
#include<string.h>
#include <stdlib.h>
#include <time.h>


#define NUMBER_OF_FLOORS 12
#define SIZE 3
#define UP 1
#define IN 1
#define DOWN 0


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
    int process_in_elevator[SIZE];
    int actual_floor;

} Elevator;

void show_elevator(Elevator elevator) {
    for (int i = 1; i < SIZE; ++i) {
        printf("Process number %d:\n", i);
        printf("\televator.process_localization %d\n", elevator.process_localization[i]);
        printf("\televator.direction %d\n", elevator.direction[i]);
        printf("\televator.process_end_floor %d\n", elevator.process_end_floor[i]);
        printf("\televator.process_in_elevator %d\n", elevator.process_in_elevator[i]);
    }
    printf("actual_floor %d\n", elevator.actual_floor);
}

void call_elevator(int rank, int start_floor, int direction, int end_floor, Elevator *elevator) {
//elevetor get information where is process and in which direction want go
//  0 - down, 1 - up

    (*elevator).process_localization[rank] = start_floor;
    (*elevator).direction[rank] = direction;
    (*elevator).process_end_floor[rank] = end_floor;
}


void set_start_variables_in_elevator(int size, Elevator *elevator) {
    for (int j = 1; j <= size - 1; ++j) {
        (*elevator).process_localization[j] = 0;
        (*elevator).direction[j] = 0;
        (*elevator).process_end_floor[j] = 0;
        (*elevator).process_in_elevator[j] = 0;

    }
    (*elevator).actual_floor = 0;
}

void set_start_and_end_floor(int size, int tag, int *start_floor, int *end_floor) {
    for (int j = 0; j < size - 1; ++j) {
        (*start_floor) = rand_int(0, NUMBER_OF_FLOORS);
        (*end_floor) = rand_int(0, NUMBER_OF_FLOORS);
        while (*start_floor == *end_floor)
            (*end_floor) = rand_int(0, NUMBER_OF_FLOORS);

        MPI_Send(start_floor, 1, MPI_INT, j + 1, tag, MPI_COMM_WORLD);
        MPI_Send(end_floor, 1, MPI_INT, j + 1, tag, MPI_COMM_WORLD);
    }
}


void move_elevator_on_the_highest_request_floor_with_entering_passengers(Elevator *elevator) {
//    find_highest_requested_floor
    int max = -1;
    printf("max %d\n", max);
    for (int i = 1; i < SIZE; ++i) {
        if ((*elevator).process_end_floor[i] > max) max = (*elevator).process_end_floor[i];
        if ((*elevator).process_localization[i] > max) max = (*elevator).process_localization[i];
    }
    show_elevator(*elevator);
//  go to the passenger who want to go to the highest floor and catch they who want to go up.
    while ((*elevator).actual_floor != max +1) {
        for (int process = 1; process < SIZE; ++process) {
            if ((*elevator).process_in_elevator[process] == IN &&
                (*elevator).actual_floor == (*elevator).process_end_floor[process]) {
                (*elevator).process_in_elevator[process] = 0;
                printf("\n\t\t\t\t\t\t\t\t\tProces %d wysiada na piętrze %d pietro zamierzone %d \n", process,
                       (*elevator).actual_floor, (*elevator).process_end_floor[process]);
            }
        }

        for (int process = 1; process < SIZE; ++process) {
            if ((*elevator).process_localization[process] == elevator->actual_floor &&
                (*elevator).process_localization[process] != (*elevator).process_end_floor[process] &&
                (*elevator).direction[process] == UP && (*elevator).process_in_elevator[process] == 0) {
//                enter passenger and mark this  in elevator variables.
                printf("\n\t\t\t\t\t\t\t\t\tProces %d wsiada na piętrze %d pietro zamierzone %d \n", process,
                       (*elevator).actual_floor, (*elevator).process_end_floor[process]);

                (*elevator).process_in_elevator[process] = 1;
            }
        }
        (*elevator).actual_floor += 1;
        for (int process = 1; process < SIZE; ++process) {
            if ((*elevator).process_in_elevator[process] == 1)
                (*elevator).process_localization[process] = (*elevator).actual_floor;
        }
    }
//    Go Down and
    int min = NUMBER_OF_FLOORS + 1;
    for (int i = 1; i < SIZE; ++i) {
        if ((*elevator).process_end_floor[i] < min) min = (*elevator).process_end_floor[i];
    }
    printf("min %d\n", min);

    for (int i = 2; i < SIZE; ++i) {
        if ((*elevator).process_localization[i] > max) { max = (*elevator).process_localization[i];
        }
        else if ((*elevator).process_localization[i] < min) min = (*elevator).process_localization[i];
    }
//  go to the passenger who want to go to the highest floor and catch they who want to go up.
    while ((*elevator).actual_floor != min -1) {
        for (int process = 1; process < SIZE; ++process) {
            if ((*elevator).process_in_elevator[process] == IN &&
                (*elevator).actual_floor == (*elevator).process_end_floor[process]) {
                (*elevator).process_in_elevator[process] = 0;
                printf("\n\t\t\t\t\t\t\t\t\tProces %d wysiada na piętrze %d pietro zamierzone %d \n", process,
                       (*elevator).actual_floor, (*elevator).process_end_floor[process]);
            }
        }

        for (int process = 1; process < SIZE; ++process) {
            if ((*elevator).process_localization[process] == elevator->actual_floor &&
                (*elevator).process_localization[process] != (*elevator).process_end_floor[process] &&
                (*elevator).direction[process] == DOWN && (*elevator).process_in_elevator[process] == 0) {
//                enter passenger and mark this  in elevator variables.
                printf("\n\t\t\t\t\t\t\t\t\tProces %d wsiada na piętrze %d pietro zamierzone %d \n", process,
                       (*elevator).actual_floor, (*elevator).process_end_floor[process]);

                (*elevator).process_in_elevator[process] = 1;
            }
        }
        (*elevator).actual_floor -= 1;
        for (int process = 1; process < SIZE; ++process) {
            if ((*elevator).process_in_elevator[process] == 1)
                (*elevator).process_localization[process] = (*elevator).actual_floor;
        }
    }
}

int main(int argc, char **argv) {
    printf("\n\n\n\n\n");
    srand(time(NULL));

    int rank, size, r;
    int src, dst, tag, i;
    MPI_Status status;

    MPI_Datatype new_type;
    MPI_Datatype type[5] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    int blen[5] = {SIZE, SIZE, SIZE, SIZE, 1};
    MPI_Aint disp[5];
    MPI_Aint base, addr;
    Elevator elevator;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != SIZE) {
        printf("set proper SIZE");
        exit(0);
    }
    MPI_Get_address(&elevator, &base);
    MPI_Get_address(&(elevator.process_localization), &addr);
    disp[0] = addr - base;
    MPI_Get_address(&(elevator.direction), &addr);
    disp[1] = addr - base;
    MPI_Get_address(&(elevator.process_end_floor), &addr);
    disp[2] = addr - base;
    MPI_Get_address(&(elevator.process_in_elevator), &addr);
    disp[3] = addr - base;
    MPI_Get_address(&(elevator.actual_floor), &addr);
    disp[4] = addr - base;

    MPI_Type_create_struct(5, blen, disp, type, &new_type);
    MPI_Type_commit(&new_type);

    int start_floor;
    int end_floor;


    if (rank == 0) {

        set_start_and_end_floor(size, tag, &start_floor, &end_floor);


        set_start_variables_in_elevator(size, &elevator);

        for (int j = 0; j < size - 1; ++j) {
            MPI_Send(&elevator, 1, new_type, j + 1, tag, MPI_COMM_WORLD);
            MPI_Recv(&elevator, 1, new_type, j + 1, tag, MPI_COMM_WORLD, &status);
        }


    } else {
        MPI_Recv(&start_floor, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&end_floor, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        int direction = end_floor > start_floor ? 1 : 0;

        MPI_Recv(&elevator, 1, new_type, 0, tag, MPI_COMM_WORLD, &status);

        call_elevator(rank, start_floor, direction, end_floor, &elevator);
        MPI_Send(&elevator, 1, new_type, 0, tag, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);


    if (rank == 0) {
        move_elevator_on_the_highest_request_floor_with_entering_passengers(&elevator);

    }


//    if (rank == 0) {
//        show_elevator(elevator);
//    }
    MPI_Finalize();
    return 0;
}
