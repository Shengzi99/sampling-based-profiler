#include<stdio.h>
#include<unistd.h>
#include<dlfcn.h>
#include<mpi.h>

extern void sbProf_start();
extern void sbProf_stop();

void foo(){
    int tmp=0;
    for(int i=0;i<100000000;i++) tmp+=i;
}
void bar(){
    int tmp=0;
    for(int i=0;i<500000000;i++) tmp+=i;
}
void foobar(){
    int tmp=0;
    for(int i=0;i<1000000000;i++) tmp+=i;
}
int main(){

    printf("test start\n");

    sbProf_start();
    
    MPI_Init(NULL, NULL);
    int my_rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    int n=1;
    while(n--){
        foo();
        bar();
        foobar();
    }    

    sbProf_stop();

    int send_buff = my_rank;
    int recv_buff = 0;

    MPI_Request rq;
    MPI_Isend(&send_buff, 1, MPI_INT, (my_rank+1)%size, 99, MPI_COMM_WORLD, &rq);
    MPI_Recv(&recv_buff, 1, MPI_INT, (size+my_rank-1)%size, 99, MPI_COMM_WORLD, NULL);

    MPI_Barrier(MPI_COMM_WORLD);
    printf("Rank: %d, Send: %d, Recv: %d\n", my_rank, send_buff, recv_buff);

    MPI_Finalize();
    return 0;
}