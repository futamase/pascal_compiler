#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unordered_map>
#include <iostream>
#include <memory>

#include "compiler.h"
#include "define.h"
#include "output.h"
#include "sym_table.h"
#include "expression.h"
#include "translation.h"



#define PRINT(s) std::cout << s << std::endl

extern TOKEN tok;
extern FILE *infile;
extern FILE *outfile;
extern Node nodes[100];
extern int node_count;
static int register_num;

std::unordered_map<std::string, FuncData> func_data_list;
bool is_main = false;
extern int recurrent;

extern SymData data_labels[500];
extern int label_counter;

static int while_label_num = 0;
static int if_label_num = 0;

namespace{
	void error(const std::string& s) {
		std::cerr << s << std::endl;
		exit(1);
	}
}

namespace p1{
	Program::Program(){
		init_getsym();
	}
	void Program::compile(){
		// programを読み込み
		getsym();
		if (tok.attr != RWORD || tok.value != PROGRAM)
			error("At the first, program declarationi is required.");

		// プログラム名を読み込み
		getsym();
		if (tok.attr != IDENTIFIER)
			error("Program identifier is needed.");
		m_name = std::string(tok.charvalue);

		// プログラム名のあとのセミコロンを読み込み
		getsym();
		if (tok.attr != SYMBOL || tok.value != SEMICOLON)
			error("After program name, a semicolon is needed.");

		// 本体読み込み>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		getsym();
		outblock();
		// 本体読み込み<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		getsym();
		// 式が正常に終わっているか確認
		if (tok.attr != SYMBOL || tok.value != PERIOD)
			error("At the end, a period is required.");

		PRINT("Parsing Completed. No errors found.");
	}

	void Program::outblock(){
		if (tok.attr == RWORD && tok.value == VAR)
			this->read_global_var();

		if (tok.attr == RWORD && tok.value == PROCEDURE) {
			do{
				getsym();
				// 関数名登録
				std::string funcName(tok.charvalue);

				getsym();
				inblock(funcName);

				if(tok.attr != SYMBOL || tok.value != SEMICOLON)
				std::cout << tok.charvalue << std::endl,error("illegal bunpou in outblock");

				getsym();
				if(tok.value != PROCEDURE)
					break;

			}while(1);
		}

		is_main = true;
	  auto st = statement();
	  // st->output();
		st->dump();

		std::cout << m_proc_map.size() << std::endl;
		for(auto&& proc : m_proc_map){
				proc.second.m_block->dump();
		}
	}

	void Program::inblock(const std::string& func_name){
	  int arg_count = 0, var_ = 0;
	  if(tok.attr != SYMBOL || tok.value != LPAREN)
	    error("illegal bunpou in inblock");

	  getsym();
	  if(tok.attr == IDENTIFIER){
	    do{
	      arg_count++;
				m_proc_map[func_name].add_arg(tok.charvalue);

	      // , を期待して読み込む
	      getsym();
	      if(tok.value != COMMA)
					break;

	      getsym();
	    }while(1);
	  }

	  if(tok.value != RPAREN)
	    error("illegal bunpou");

	  getsym();
	  if(tok.value != SEMICOLON)
	    error("illegal bunpou");

	  getsym();
	  // ローカル変数が定義されていたら読み込む
	  if(tok.attr == RWORD && tok.value == VAR){
	    do{
	      getsym();
				m_proc_map[func_name].add_local_var(tok.charvalue);
	      var_++;
	      getsym();

	      if(tok.value == COMMA)
	        continue;
	      else if(tok.value == SEMICOLON)
	        break;
	      else
	        error("illegal bunpou");
	    }while(1);
	  }

		// statement の始まりを読み込む
	  getsym();

		auto block = this->statement(&m_proc_map[func_name]);
		std::cout << m_proc_map.size() <<std::endl;
		m_proc_map[func_name].setBlock(std::move(block));

	  getsym();
	}

	/* 式を読み込み、その式を計算し結果を表示するコードを生成する処理*/
	std::unique_ptr<Statement> Program::statement(const Procedure* caller) {
		if(tok.attr == RWORD){
			if (tok.value == BEGIN) {
				Block block;
				block.set_caller(caller);

				do {
					getsym();
					auto s = this->statement(caller);
					// std::cout << "added state is " << (int)s->getType() << std::endl;
					block.add(std::move(s));
				} while (tok.value == SEMICOLON);

				if (tok.value != END)
					error("must need end");

				return std::make_unique<Block>(std::move(block));
			}
			else if (tok.value == IF) {
				If ifobj;
				ifobj.set_caller(caller);

				// sprintf(label_buffer, "elseLabel%d", if_label_counter_register);
				// condition(label_buffer);
				ifobj.setCondition(this->condition("label"));

				if(tok.attr != RWORD || tok.value != THEN)
				error("if neccesary then");

				getsym();
				auto thenBlock = this->statement();
				ifobj.setThenBlock(std::move(thenBlock));

				// 糞コードを極めていくスタイル
				// if 部がbegin-endのときにelseが読み込まれなかった
				if(tok.attr == RWORD && tok.value == END)
				getsym();

				if(tok.attr == RWORD && tok.value == ELSE){
					getsym();

					auto elseBlock = this->statement();
					ifobj.setElseBlock(std::move(elseBlock));
				}

				return std::make_unique<If>(std::move(ifobj));
			}
			else if (tok.value == WHILE) {
				While whileobj;
				whileobj.set_caller(caller);

				auto cond = this->condition("label");
				whileobj.setCondition(cond);

				if(tok.attr != RWORD || tok.value != DO)
					error("while neccesary do");

				getsym();
				auto&& whileblock = this->statement();
				whileobj.setBlock(std::move(whileblock));
				getsym();

				return std::make_unique<While>(std::move(whileobj));
			}
			else if(tok.value == WRITE){

				Write write;
				write.set_caller(caller);
				write.read();
				return std::make_unique<Write>(std::move(write));
			}
		}
		else{
	    // if(!get_symbol(tok.charvalue, &symbol))
	    //   error("identifier was'nt found");
	    if(m_proc_map.find(tok.charvalue) != m_proc_map.end()){
	      auto proc = this->paramlist(tok.charvalue);
				proc.set_caller(caller);
				return std::make_unique<CalledProcedure>(std::move(proc));
	    }
	    else{
				std::string name(tok.charvalue);
	      getsym();
	      if (tok.value != BECOMES)
		      error("illegal bunpou");

				Assignment assignment(name);
				assignment.set_caller(caller);
				return std::make_unique<Assignment>(std::move(assignment));
	    }
		}
	}

 Condition Program::condition(const std::string& label){
		Condition cond;
		cond.m_lhs.read();

		if(tok.attr != SYMBOL)
			error("illegal bunpou");

		cond.m_symbol = tok.value;
		cond.m_symbol_str = std::string(tok.charvalue);

		cond.m_rhs.read();
		return cond;
	}

	CalledProcedure Program::paramlist(const std::string& func_name){
		CalledProcedure proc;
//		int regi;
//		int i, j;
//		Node* nodes_storage;
//		int* node_count_storage;
//		int arg_num = 0;
//
		getsym();
		if(tok.value != LPAREN)
		error("func must begin '('");

		do{
//			init_nodes();
			proc.m_arg.emplace_back();
			proc.m_arg.back().read();
//			flush_stack();
//
//			arg_num++;
//
			if(tok.value != COMMA){
				break;
			}
		}while(1);
//
		if(tok.value != RPAREN)
			error("func must end ')'");
//
		getsym();

		return proc;
	}

	// グローバル変数読み込み
	void Program::read_global_var() {
		int count = 0;
		do{
			getsym();
			if (tok.attr != IDENTIFIER)
				error("not var");

			// 追加
			m_global_var_map.emplace(tok.charvalue, count++);

			getsym();

			if (tok.value == COMMA)					continue;
			else if(tok.value == SEMICOLON)	break;
			else error("need comma or semicolon");

		} while (1);

	  // flush_all_symbols();
		//セミコロン後、１ワード取り出す
		getsym();
	}
}
