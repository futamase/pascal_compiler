#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>
#include <vector>
#include <fstream>
#include "sym_table.h"

struct Command{
  std::string opecode;
  std::string operand_l;
  std::string operand_r;
  Command(const std::string& op, const std::string& l, const std::string& r)
    : opecode(op), operand_l(l), operand_r(r)
  {}
};

namespace p1{
  class Commander {
    std::vector<Command> m_commands;
    Commander() = default;
    Commander(const Commander&) = delete;
    Commander& operator= (const Commander&) = delete;
  public:
    static Commander& get_instance() {
      static Commander instance;
      return instance;
    }
    void add_command(const Command& command){
      m_commands.emplace_back(command);
    }
    void output_all(std::fstream& outfile){
      for(auto&& command : m_commands){
        outfile << command.opecode << "\t" << command.operand_l << "," << command.operand_r << std::endl;
      }
    }
  };
}

struct FuncData{
	std::vector<SymData> local_vars;
  std::vector<Command> push_commands;
  int getArgNum() const {
    int i;
    for(i = 0; i < local_vars.size() && local_vars.at(i).attr != Local; i++);
    return i;
  }
	std::string getArgName(int argNum){
		return local_vars.at(argNum).name;
	}
};

void set_p_funt(std::vector<Command>* p);

void add_op(const std::string& opecode, const std::string& operand_l, const std::string& operand_r);

void output_all(FILE* outfile);

void inline_function(const std::string& func_name, int rec_num, int multi_call_num = 0);

#endif
