#ifndef HOGE_H
#define HOGE_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "expression.h"

namespace p1{

    struct Condition{
      Expression m_lhs, m_rhs;
      std::string m_symbol_str;
      int m_symbol;
    };

    class Procedure;

    // 式の基底クラス
    class Statement{
      public:
        enum class Type{
          Assignment = 0,
          Block,
          If,
          While,
          CalledProcedure,
          Write,
        };

      protected:
        Type m_type;
        const Procedure* m_caller = nullptr;
      public:
        void set_caller(const Procedure* caller){
          m_caller = caller;
        }
        virtual ~Statement(){}
        Type getType() const { return m_type; }
        virtual void output() const {}
        virtual void dump() {}
    };

    class Assignment : public Statement{
      std::string m_lhs;
      Expression m_rhs;

    public:
      Assignment();
      Assignment(const std::string& lhs);

      void setLHS(const std::string& lhs) { m_lhs = lhs; }
      void setRHS(const Expression&& rhs) { m_rhs = std::move(rhs); }

      const std::string& getLHS() const { return m_lhs; }
      const Expression& getRHS() const { return m_rhs; }
      virtual void output() const{
        std::cout << "var " << m_lhs << "was assigned " << std::endl;
        m_rhs.output();
      }
      void dump() override;
    };
    class Block : public Statement {
    public:
      std::vector<std::unique_ptr<Statement>> m_statements;
      Block(){
        m_type = Statement::Type::Block;
      }
      Block(Block&& r){
        m_type = Statement::Type::Block;
        this-> m_statements = std::move(r.m_statements);
      }
      void add(std::unique_ptr<Statement>&& state){
        m_statements.emplace_back(std::move(state));
      }
      void output() const override {
        std::cout << "block size " << m_statements.size() << std::endl;
        for(auto&& i : m_statements){
          i->output();
        }
      }
      void dump() override {
        for(auto&& i : m_statements){
          i->dump();
        }
      }
    };
    class If : public Statement{
    public:
      Condition m_cond;
      std::unique_ptr<Statement> m_then_block;
      std::unique_ptr<Statement> m_else_block;
      If();
      void setCondition(const Condition& cond){
        m_cond = cond;
      }
      void setThenBlock(std::unique_ptr<Statement>&& block){
        m_then_block = std::move(block);
      }
      void setElseBlock(std::unique_ptr<Statement>&& block){
        m_else_block = std::move(block);
      }
      void output() const override {
        std::cout << std::endl;
        std::cout << "if begin" << std::endl;
        std::cout << "condition:\nleft:";
        m_cond.m_lhs.output();
        std::cout << "op is" << m_cond.m_symbol_str << std::endl;
        std::cout << "right:";
        m_cond.m_rhs.output();

        std::cout << "then block begin" << std::endl;
        m_then_block->output();
        std::cout << "then block end" << std::endl;
        std::cout << "else block begin" << std::endl;
        if(m_else_block)
          m_else_block->output();
        std::cout << "else block end" << std::endl;
        std::cout << "if end" << std::endl;
      }
    };
    class While : public Statement{
    public:
      Condition m_cond;
      std::unique_ptr<Statement> m_block;
      While();
      void setCondition(const Condition& cond){
        m_cond = cond;
      }
      void setBlock(std::unique_ptr<Statement>&& block){
        m_block = std::move(block);
      }
      void output() const override{
        std::cout << std::endl;
        std::cout << "while begin" << std::endl;
        std::cout << "condition:\nleft:";
        m_cond.m_lhs.output();
        std::cout << "op is" << m_cond.m_symbol_str << std::endl;
        std::cout << "right:";
        m_cond.m_rhs.output();

        std::cout << "block begin" << std::endl;
        m_block->output();
        std::cout << "block end" << std::endl;
        std::cout << "while end" << std::endl;
      }
    };
    // 式として評価するときの関数
    class CalledProcedure : public Statement{
    public:
      std::string m_name;
      std::vector<Expression> m_arg;
      CalledProcedure();
    };
    class Write : public Statement {
    public:
      std::vector<std::string> m_vars;
      Write();
      void read();
      void output() const override {
        std::cout << "=====THE write var======" << std::endl;
        for(auto&& i : m_vars){
          std::cout << i << std::endl;
        }
        std::cout << "=====THE write var======" << std::endl;
      }
    };

    // 実態としての関数
    class Procedure {
    public:
      std::vector<std::string> m_args;
      std::vector<std::string> m_local_vars;
      std::unique_ptr<Statement> m_block;
      void add_arg(const std::string& arg){
        m_args.emplace_back(arg);
      }
      void add_local_var(const std::string& var){
        m_local_vars.emplace_back(var);
      }
      void setBlock(std::unique_ptr<Statement>&& b){
        m_block = std::move(b);
      }
      bool find_var(const std::string& s){
        for(auto&& arg : m_args){
          if(arg == s)
            return true;
        }
        for(auto&& var : m_local_vars){
          if(var == s)
            return true;
        }

        return false;
      }
    };
}

#endif
