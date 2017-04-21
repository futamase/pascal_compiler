#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>
#include <string>
#include <iostream>

void init_nodes(void);
void expression(void);
void flush_stack(void);

// for debug
void output_nodes(void);

namespace p1{
    // 4つ組構造体
    struct Tetrad{
      std::string opcode;
      std::string op_r;
      std::string op_l;
      int dest;
      Tetrad() = default;
      Tetrad(const std::string& _op, const std::string& r, const std::string& l, int _dest){
        opcode = _op;
        op_r = r;
        op_l = l;
        dest = _dest;
      }
    };

    // 式は４つ組の配列で表現
    class Expression {
        std::vector<std::string> m_stack;
        std::vector<Tetrad> m_tetrads;

        int m_register_count = 0;
      private:
        void factor();
        void term();
        void expr();
        void end();
        void ranking_function(const std::string& a_j);
        void push_tetra(const std::string& op, const std::string& l, const std::string& r);
        
      public:
        Expression();
        void read();
        void output()const {
          std::cout << m_tetrads.size() << std::endl;
          for(auto&& tetra : m_tetrads){
            std::cout << tetra.opcode << " " << tetra.op_r << " " << tetra.op_l << std::endl;
          }
        }
        std::vector<Tetrad> getTetrads() const { return m_tetrads; }
    };
}

#endif
