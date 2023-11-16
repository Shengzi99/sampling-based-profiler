#include<stdio.h>
#include<unistd.h>
#include<dlfcn.h>
#include<string.h>
#include<stdlib.h>

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
    foo();
    bar();
}
int main(){

    printf("test start\n");

    sbProf_start();

    char *tmp;
    int n=1;
    while(n--){
        foo();
        bar();
        foobar();
    }    

    sbProf_stop();

    return 0;
}