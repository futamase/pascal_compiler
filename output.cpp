#include "output.h"
#include "define.h"
#include "sym_table.h"
#include <vector>
#include <unordered_map>
#include <iostream>
/*
extern std::unordered_map<std::string, FuncData> func_data_list;
extern SymData data_labels[500];
extern int label_counter;

static std::vector<Command> command_list;
static std::vector<Command>* p_func_command_list;


void set_p_funt(std::vector<Command>* p){
  p_func_command_list = p;
}

void add_op(const std::string& opecode, const std::string& operand_l, const std::string& operand_r)
{
  command_list.emplace_back(opecode, operand_l, operand_r);
  if(p_func_command_list)
    p_func_command_list->emplace_back(opecode, operand_l, operand_r);
}

static void delete_waste_load(){
  for(auto ite = command_list.begin(); ite != command_list.end()-1; ++ite){
    if(ite->operand_l == (ite+1)->operand_l &&
      ite->operand_r == (ite+1)->operand_r &&
      ite->opecode == "store" && (ite+1)->opecode == "load")
    {
        ite = command_list.erase(ite+1);
    }
  }
}


void erase_waste_store(const std::string& regi){
  for(auto ite = command_list.begin(); ite != command_list.end(); ++ite){
    if(ite->opecode == "store" && ite->operand_r == regi){
      command_list.erase(ite);
      return; // 消すのはひとつだけ
    }
  }
}

int recurrent = 1;
int max_recurrent = 2;
void inline_function(const std::string& func_name, int rec_num, int multi_call_num){
  // hanoi対策
  int call_count = 0;

  if(rec_num > max_recurrent)return;

  int cur_arg = func_data_list.at(func_name).getArgNum();
  for(int i = 0; i < cur_arg; i++)
			sprintf(data_labels[label_counter++].name, "il%d_%d%s: data 0",
        rec_num, multi_call_num, func_data_list.at(func_name).local_vars.at(i).name);

  int size =command_list.size();
  for(int i = 0; i < size; ++i){
    if(command_list.at(i).opecode.find(func_name) != std::string::npos){
      for(int j = i+4; ; j++){
        if(command_list.at(j+2).opecode == "return"){
          return;
        }

        if(rec_num < max_recurrent-1 && command_list.at(j).opecode == "push"){
          auto funcData = func_data_list.at(func_name);
          auto name = funcData.local_vars.at(cur_arg-1).name;

          add_op("store", command_list.at(j).operand_l, "il" + std::to_string(rec_num) + "_" + std::to_string(call_count) + name);
          cur_arg--;
          continue;
        }

        if(command_list.at(j).opecode == "call"){
          cur_arg = func_data_list.at(func_name).getArgNum();
          inline_function(func_name, rec_num+1, call_count++);
          std::cout << "このすば！"  << call_count<< std::endl;
        }

        auto c = command_list.at(j);

        if(c.operand_r.find("(BR)") != std::string::npos)
        {
          c.operand_r.erase(c.operand_r.find_first_of("(BR)"), 4);
          int addr = atoi(c.operand_r.data());
          for(auto&& hoge : func_data_list.at(func_name).local_vars){
            if(hoge.addr == addr){
              if(c.opecode == "load")
                c.operand_r = "il" + std::to_string(rec_num) + "_" + std::to_string(call_count) + std::string(hoge.name);
              else if(c.opecode == "store")
                c.operand_r = "il" + std::to_string(rec_num+1) + "_" + std::to_string(call_count) + std::string(hoge.name);

              break;
            }
          }
        }
        else if(c.opecode.find("while") != std::string::npos ||
                c.opecode.find("if") != std::string::npos ||
                c.opecode.find("else") != std::string::npos){
            c.opecode.insert(0, "il" + std::to_string(rec_num)+ "_" + std::to_string(call_count));
        }
        else if(c.operand_l.find("while") != std::string::npos ||
                c.operand_l.find("if") != std::string::npos ||
                c.operand_l.find("else") != std::string::npos){
            c.operand_l.insert(0, "il"+std::to_string(rec_num) + "_" + std::to_string(call_count));
        }

        add_op(c.opecode, c.operand_l, c.operand_r);
      }
    }
  }

  recurrent++;
}

// storeが一回しかされていないものは
// store されてもloadがないものはいらない
void replace_something(){
  std::unordered_map<std::string, int> var_store_list;
  std::unordered_map<std::string, int> var_load_list;

  for(int i = 0; i < command_list.size(); i++){
    if(command_list.at(i).opecode == "store"){
      var_store_list[command_list.at(i).operand_r]++;
    }
    else if(command_list.at(i).opecode == "load"){
      var_load_list[command_list.at(i).operand_r]++;
    }
  }

  for(auto&& i : var_store_list){
      if(var_load_list.find(i.first) == var_load_list.end()){
        erase_waste_store(i.first);
      }
  }


  std::cout << "###########################" <<std::endl;
  for(auto&& i : var_store_list){
    std::cout << i.first << " " << i.second  << std::endl;
  }
  std::cout << "###########################" <<std::endl;
  for(auto&& i : var_load_list){
    std::cout << i.first << " " << i.second  << std::endl;
  }
  std::cout << "###########################" <<std::endl;
}

void output_all(FILE* file){
  delete_waste_load();

//  replace_something();
for(auto&& d : func_data_list){
    std::cout << "##########" << d.first << "############" << std::endl;
    for(auto&& com : d.second.push_commands){
      std::cout << com.opecode << " " << com.operand_l << " " << com.operand_r << std::endl;
    }
    std::cout << "##########end " << d.first << "############" << std::endl;
}

  for(auto&& command : command_list){
    fprintf(file, "%s", command.opecode.c_str());

    if(command.operand_l.empty()){
      fprintf(file, "\n");
      continue;
    }

    fprintf(file, "\t%s", command.operand_l.c_str());

    if(command.operand_r.empty()){
      fprintf(file, "\n");
      continue;
    }

    fprintf(file, ", %s\n", command.operand_r.c_str());
  }
}
*/
