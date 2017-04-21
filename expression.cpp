#include "expression.h"
#include "utility.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <iostream>
#include <algorithm>

#include <getsym.h>
extern TOKEN tok;

using namespace util;

namespace {

  enum class Rank{
    Greater,
    Less,
    Equal
  };

  int f(const std::string& s){
    static std::unordered_map<std::string, int> s_op_f_list {
      { "+", 2 },
      { "-", 2 },
      { "*", 4 },
      { "div", 4 },
      { "!", 6 },
      { "(", 0 },
      { ")", 11 },
      { "$", 0 }
    };
    return s_op_f_list.at(s);
  }

  int g(const std::string& s){
    static std::unordered_map<std::string, int> s_op_g_list {
      { "+", 1 },
      { "-", 1 },
      { "*", 3 },
      { "div", 3 },
      { "!", 15 },
      { "(", 10 },
      { ")", 0 },
      { "$", 0 }
    };
    return s_op_g_list.at(s);
  }

  Rank ranking(int f, int g){
    return f > g  ?   Rank::Greater :
           f == g ?   Rank::Equal :
        /* f <  g ?*/ Rank::Less;
  }
}

namespace p1
{
  Expression::Expression()
  {
    m_stack.emplace_back("$");
  }

  void Expression::factor(){
    // 単項演算子-の時
    if(tok.attr == SYMBOL && tok.value == MINUS){
  		  getsym();
        this->ranking_function("!");
    }

    if(tok.attr == IDENTIFIER){
        m_stack.emplace_back(tok.charvalue);
    }
    else if(tok.attr == NUMBER){
        m_stack.emplace_back(tok.charvalue);
    }
    else if(tok.attr == SYMBOL && tok.value == LPAREN){
        ranking_function(tok.charvalue);
        this->expr();

        if(tok.value != RPAREN)
          fprintf(stderr, "illegal bunpouなのだ\n"), exit(1);
        ranking_function(tok.charvalue);
    }
  }

  void Expression::term(){
    do{
      this->factor();
      getsym();

      if(tok.attr == RWORD && tok.value == DIV){
        this->ranking_function(tok.charvalue);
        getsym();
      }
      else if(tok.attr == SYMBOL&& tok.value == TIMES) {
        this->ranking_function(tok.charvalue);
        getsym();
      }
      else{
        break;
      }
    }while(1);
  }

  void Expression::expr(){
     do{
       getsym();
       this->term();

       if(tok.attr == SYMBOL && tok.value == PLUS){
         this->ranking_function(std::string(tok.charvalue));
       }
       else if(tok.attr == SYMBOL && tok.value == MINUS){
         this->ranking_function(std::string(tok.charvalue));
       }
       else{
         break;
       }
     }while(1);
  }

  void Expression::end(){
     this->ranking_function("$");
  }

  void Expression::read(){
    this->expr();
    this->end();
  }

  int calculate_const_value(const std::string& op, int left, int right){
    static std::unordered_map<std::string, std::function<int(int, int)>> s_const_func {
      {"+",   [](int l, int r){ return l + r; }},
      {"-",   [](int l, int r){ return l - r; }},
      {"*",   [](int l, int r){ return l * r; }},
      {"div", [](int l, int r){ return l / r; }},
    };
    return s_const_func.at(op)(left, right);
  }

  void Expression::push_tetra(const std::string& op, const std::string& l, const std::string& r){
    if(isDigit(l) && isDigit(r)){
      auto result = calculate_const_value(op, std::stoi(l), std::stoi(r));
      m_stack.emplace_back(std::to_string(result));
      std::cout << "つんだ " << m_stack.back() << std::endl;
      return;
    }

    m_tetrads.emplace_back();
    auto& new_tetra = m_tetrads.back();
    new_tetra.opcode = op;
    new_tetra.op_r = r;
    new_tetra.op_l = l;
    new_tetra.dest = m_register_count++;

    m_stack.emplace_back("__r" + std::to_string(new_tetra.dest));
  }

  void Expression::ranking_function(const std::string& a_j){
    auto a_i = std::find_if(m_stack.rbegin(), m_stack.rend(),
      [](const std::string& op){ return isOperator(op); });

    while(true){
      if(a_i == m_stack.rend())
        std::cerr << "ないよ" << std::endl, exit(1);

      auto rank = ranking(f(*a_i), g(a_j));

      if(rank == Rank::Greater){
        auto ope_r = m_stack.back(); m_stack.pop_back();
        auto op = m_stack.back(); m_stack.pop_back();
        auto ope_l = m_stack.back(); m_stack.pop_back();

        this->push_tetra(op, ope_l, ope_r);

        a_i = std::find_if(m_stack.rbegin(), m_stack.rend(),
              [](const std::string& op){ return isOperator(op); });
      }
      else if(rank == Rank::Less){
        m_stack.emplace_back(a_j);
        break;
      }
      else if(rank == Rank::Equal){
        if(*a_i == "("){
            m_stack.erase(--(a_i.base()));
          break;
        }
        else if(*a_i == "$"){
          if(!isRegister(m_stack.back()))
          {
            m_tetrads.emplace_back();
            m_tetrads.back().op_l = m_stack.back();
            m_tetrads.back().dest = "__r0";
          }
          break;
        }
      }
    }
  }
}
