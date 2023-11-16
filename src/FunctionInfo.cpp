#include "FunctionInfo.h"
#include "utils.h"
#include "defs.h"
#include<string.h>

void FunctionInfo::insert_caller(unw_word_t pc){
    if(callers.count(pc))
        callers[pc] ++;
    else
        callers[pc] = 0;
}

void ProfGraph::insert_one_stack(unw_word_t *pc_stack, char **sym_stack, size_t count){
    if(count<1) return;

    unw_word_t pc;
    char *sym;
    int idx = count;
    
    while(idx--){
        pc = pc_stack[idx];
        sym = sym_stack[idx];
        if(_graph.count(pc)){
            _graph[pc]->pc_count_self+=(idx?0:1);
            _graph[pc]->pc_count_total++;
        }
        else{  
            strcpy(_func_list[_func_list_size].sym, sym);
            _func_list[_func_list_size].pc_count_self=(idx?0LLU:1LLU);
            _func_list[_func_list_size].pc_count_total=1;
            _func_list[_func_list_size].pc = pc;
            _graph[pc] = &_func_list[_func_list_size++];
        }
        if(idx<count-1) _graph[pc]->insert_caller(pc_stack[idx+1]);
    }
    pc_total ++;
    return;
}

bool cmp_pc_count_self(const FunctionInfo* a, const FunctionInfo* b){
    return a->pc_count_self>b->pc_count_self;
}
bool cmp_pc_count_total(const FunctionInfo* a, const FunctionInfo* b){
    return a->pc_count_total>b->pc_count_total;
}

void ProfGraph::print_stat(){
    double one_pc_time = (wt_total-wt_prof)/pc_total;
    putchar('\n');
    printf("================================ sbProf ========================================================================================================================\n");
    printf("Total run time: %lfs\nsbProf time cost: %lfs\nsbProf proportion: %lf%%\n", wt_total, wt_prof, 100*wt_prof/wt_total);

    printf("-------------------------------- Func Stats --------------------------------------------------------------------------------------------------------------------\n");
    printf("%8s | %15s | %-50s | %-50s | %-20s | %-20s\n", "Rank", "PC", "Symbol", "Symbol(demangled)", "Total Time(s)", "Self Time(s)");
    printf("%8s-|-%15s-|-%-50s-|-%-50s-|-%-20s-|-%-20s\n", "--------", "---------------", "--------------------------------------------------", "--------------------------------------------------", "--------------------", "--------------------");
    vector<FunctionInfo*> all_func_info;
    for(int i=0;i<_func_list_size;i++) all_func_info.push_back(_func_list+i);
    sort(all_func_info.begin(), all_func_info.end(), cmp_pc_count_total);
    for(int i=0;i<all_func_info.size();i++){
        printf("%8d | %15p | %-50s | %-50s | %-20.6f | %-20.6f\n", i, all_func_info[i]->pc,
                                                              all_func_info[i]->sym, all_func_info[i]->sym_demangled, 
                                                              all_func_info[i]->pc_count_total*one_pc_time, all_func_info[i]->pc_count_self*one_pc_time);
    }
    return;
}

void ProfGraph::save_2_dot(string fpath){
    printf("%s\n", fpath.c_str());
    return;
}
