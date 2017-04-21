#include "sym_table.h"

#include <cstdio>
#include <string.h>
#include <vector>

int var_count = 0; //グローバル化
SymData vars[MAXADDR]; // 変数の上限はないのか
int local_var_count = 0; //グローバル化
int arg_count = 0;
SymData local_vars[MAXADDR]; // 引数とローカル変数

void add_symbol(const char* name, SymAttr attr){
  switch (attr) {
    case Var:
      strcpy(vars[var_count].name, name);
      vars[var_count].attr = attr;
      vars[var_count].addr = var_count;
      var_count++;
      break;
    case Local:
      strcpy(local_vars[local_var_count].name, name);
      local_vars[local_var_count].attr = attr;
      local_vars[local_var_count].addr = local_var_count - arg_count;
      local_var_count++;
      break;
    case Arg:
      strcpy(local_vars[local_var_count].name, name);
      local_vars[local_var_count].attr = attr;
      local_vars[local_var_count].addr = -(arg_count + 1) - 1;
      local_var_count++;
      arg_count++;
      break;
    case Func:
      strcpy(vars[var_count].name, name);
      vars[var_count].attr = attr;
      var_count++;
      break;
  }
}

int get_symbol(const char* name, SymData* data){
  int i;
  for(i = 0; i < local_var_count; i++){
    if(strcmp(local_vars[i].name, name) == 0){
      strcpy(data->name, name);
      data->attr = local_vars[i].attr;
      data->addr = local_vars[i].addr;

      return 1;
    }
  }
  for(i = 0; i < var_count; i++){
    if(strcmp(vars[i].name, name) == 0){
      strcpy(data->name, name);
      data->attr = vars[i].attr;
      data->addr = vars[i].addr;

      return 1;
    }
  }

  return 0;
}

// 引数ありならそこにコピー
void clear_local_vars(std::vector<SymData>* p_symlist){
    if(p_symlist){
      p_symlist->resize(local_var_count);
      for(int i = 0; i < local_var_count; ++i){
        strcpy(p_symlist->at(i).name, local_vars[i].name);
        p_symlist->at(i).attr = local_vars[i].attr;
        p_symlist->at(i).addr = local_vars[i].addr;
      }
    }

    local_var_count = 0;
    arg_count = 0;
    memset(local_vars, 0, sizeof(SymData) * MAXADDR);
}


void flush_all_symbols(void){
  int i;

  for(i = 0; i < var_count; i++){
    printf("vars No.%d is: name=%s, addr=%d, attr=%d\n", i, vars[i].name, vars[i].addr, vars[i].attr);
  }
  for(i = 0; i < local_var_count; i++){
    printf("local vars No.%d is: name=%s, addr=%d, attr=%d\n", i,
      local_vars[i].name, local_vars[i].addr, local_vars[i].attr);
  }
}
