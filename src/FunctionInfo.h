#include<stdlib.h>
#include<stdint.h>
#include<cxxabi.h>
#include<vector>
#include<map>
#include<unordered_map>
#include<string>
#include<algorithm>
#include<libunwind.h>
#include "defs.h"
using namespace std;

struct FunctionInfo{
    unw_word_t pc;
    char sym[MAX_PROC_NAME_LEN];
    char sym_demangled[MAX_PROC_NAME_LEN];
    uint64_t pc_count_self;
    uint64_t pc_count_total;
    map<unw_word_t, uint64_t> callers; //key: callers pointer, value: call count
    
    void insert_caller(unw_word_t pc);
};

struct ProfGraph{
    bool started;
    timeval tv_start;
    timeval tv_stop;

    uint64_t pc_total;
    double wt_total;
    double wt_prof;
    FunctionInfo _func_list[MAX_STACK_DEPTH * 256];
    size_t _func_list_size;
    map<unw_word_t, FunctionInfo*> _graph; //adjacency list

    ProfGraph(){
        started = false;
        wt_total = 0;
        wt_prof = 0;
        _func_list_size = 0;
    }
    void insert_one_stack(unw_word_t *pc_stack, char **sym_stack, size_t count);
    void print_stat();
    void save_2_dot(string fpath);
};
