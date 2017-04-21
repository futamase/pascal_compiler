#ifndef COMPILER_H
#define COMPILER_H

#include <unordered_map>
#include <memory>
#include <string>
#include <array>
#include "newfile.h"

namespace p1{
  class Program{
  public:
    std::string m_name;
    std::unordered_map<std::string, int> m_global_var_map;
    std::unordered_map<std::string, Procedure> m_proc_map;
    std::unordered_map<int, std::string> m_const_labels;
    std::array<std::string, 4> m_register;
    void outblock();
    void inblock(const std::string& func_name);
    std::unique_ptr<Statement> statement(const Procedure* caller = nullptr);
    Condition condition(const std::string& label);
    CalledProcedure paramlist(const std::string& label);

    void read_global_var();
    Program();
  public:
    void compile();

    static Program& get_instance() {
      static Program instance;
      return instance;
    }
    const std::unordered_map<std::string, int>& get_global_var_map() const {
      return m_global_var_map;
    }
    std::string push_const_label(int num){
      static int counter = 0;
      if(m_const_labels.find(num) != m_const_labels.end())
        return m_const_labels.at(num);
      else{
        m_const_labels.emplace(num, "label" + std::to_string(counter++));
        return m_const_labels.at(num);
      }
    }
  };
}

#endif
