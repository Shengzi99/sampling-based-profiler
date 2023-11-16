#include<sys/time.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<execinfo.h>
#include<dlfcn.h>
#include<cxxabi.h>
#include<libunwind.h>
#include<vector>
#include "FunctionInfo.h"
#include "utils.h"
// #include "sbPorf.h"

using namespace std;


ProfGraph *pg;
char sym[MAX_PROC_NAME_LEN];
char* sym_stack[MAX_STACK_DEPTH];
unw_word_t pc_stack[MAX_STACK_DEPTH];

void backtrace(){
    unw_cursor_t cursor;
    unw_context_t context;
    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    int stack_count=0;
    while(unw_step(&cursor) > 0){
        unw_word_t offset, pc;

        unw_get_reg(&cursor, UNW_REG_IP, &pc); if(pc == 0) break;

        int get_proc_name_flag = unw_get_proc_name(&cursor, sym, MAX_PROC_NAME_LEN, &offset);
        if((strlen(sym)==0) || strstr(sym, "malloc")!=NULL) return;
        if( (get_proc_name_flag==0) && (strstr(sym, "sig_handler")==NULL && strstr(sym, "__sigaction")==NULL && strcmp(sym, "_start")!=0 && strstr(sym, "__libc_start_main")==NULL && strstr(sym, "__libc_init_first")==NULL) ){
            pc_stack[stack_count] = pc-offset;
            strcpy(sym_stack[stack_count++], sym);
        }
    }
    pg->insert_one_stack(pc_stack, sym_stack, stack_count);
}

void sig_handler(int signo){
    timeval tv_prof_start, tv_prof_end;
    gettimeofday(&tv_prof_start, NULL);

    switch (signo){
    case SIGALRM:
        backtrace();
        break;
    }

    gettimeofday(&tv_prof_end, NULL);
    pg->wt_prof = pg->wt_prof + TV2SEC(tv_prof_end - tv_prof_start);

    return;
}


void sbProf_start(int freq){
    pg = new ProfGraph();
    gettimeofday(&pg->tv_start, NULL);
    for(int i=0;i<MAX_STACK_DEPTH; i++) sym_stack[i] = (char*)malloc(sizeof(char)*MAX_PROC_NAME_LEN);

    if(!pg->started){
        signal(SIGALRM, sig_handler);

        itimerval itval;
        itval.it_value.tv_sec=(int)(1.0/freq);
        itval.it_value.tv_usec=(int)((1.0/freq - itval.it_value.tv_sec) * 1000000);
        itval.it_interval = itval.it_value;

        setitimer(ITIMER_REAL, &itval, NULL);

        pg->started = true;
    }
}
void sbProf_start(){
    sbProf_start(100);
}

void sbProf_stop(){
    gettimeofday(&pg->tv_stop, NULL);
    pg->wt_total = TV2SEC(pg->tv_stop - pg->tv_start);

    signal(SIGALRM, SIG_IGN);

    itimerval itval;
    itval.it_value.tv_sec=0;
    itval.it_value.tv_usec=0;
    itval.it_interval = itval.it_value;

    setitimer(ITIMER_REAL, &itval, NULL);

    char __sym_demangled[MAX_PROC_NAME_LEN];
    for(int i=0;i<pg->_func_list_size;i++){
        int demangle_flag=-999; size_t __sym_len=MAX_PROC_NAME_LEN;
        abi::__cxa_demangle(pg->_func_list[i].sym, __sym_demangled, &__sym_len, &demangle_flag);
        char *sym_demangled = (demangle_flag==0)?__sym_demangled:pg->_func_list[i].sym;
        strcpy(pg->_func_list[i].sym_demangled, sym_demangled);
    }
    pg->print_stat();
    for(int i=0;i<MAX_STACK_DEPTH; i++) free(sym_stack[i]);
    delete pg;
}