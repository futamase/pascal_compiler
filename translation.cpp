#include "translation.h"

#include "define.h"
#include "output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // for isdigit()
#include <vector>
#include <iostream>

using namespace p1;

struct Node4cpp{
  std::string op;
  std::string l;
  std::string r;
  int regi;
  Node4cpp* next;
  Node4cpp(const char* _op, const char* _l, const char* _r, int _regi)
    : op(_op), l(_l), r(_r), regi(_regi)
  {}
  Node4cpp(const std::string& _op, const std::string& _l, const std::string& _r, int _regi)
    : op(_op), l(_l), r(_r), regi(_regi)
  {}
};

static const std::array<std::string, 3> additional_op {
  "", // Var
  "(BR)",// Local
  "(BR)"// Arg
};

const std::string get_additional_op(int index){
  DebugOut2("additional is :%d\n", index);
return  additional_op.at(index);
}

extern FILE* outfile;
extern Node nodes[100];
static std::vector<Node4cpp> nodes_replaced;


extern int node_count;
extern SymData vars[MAXADDR];
extern int var_count ;

extern SymData local_vars[MAXADDR];
extern int local_var_count;

extern SymData regi_var[100];
extern int regi_var_count;

static SymData symbol;

SymData data_labels[500];
int label_counter = 0;

int isDigit(const char* s){
  int i = 0;
  DebugOut2("isDigit: %s\n", s);
  //終端文字か数字以外の文字が出てくるまでループ
  for (i = 0; s[i] != NULL && isdigit(*(s + i)); ++i);
  //ループ終了時、終端に達していなければエラー
  DebugOut2("endendend %d\n", (s[i] == NULL));
  return (s[i] == NULL);
}

int isVar(const char* s){
  int i;

  for(i = 0; i <= var_count; i++){
    if(strcmp(local_vars[i].name, s) == 0)
      return 1;
  }

  for(i = 0; i <= var_count; i++){
    if(strcmp(vars[i].name, s) == 0)
      return 1;
  }

  for(i = 0; i < regi_var_count; i++){
    if(strcmp(regi_var[i].name, s) == 0)
      return 1;
  }
  return 0;
}

int isRegister(const char* s) {
	int i;
  for(i = 0; i < regi_var_count; i++){
    if(strcmp(regi_var[i].name, s) == 0)
      return 1;
  }
  return 0;
}

// -1 のときはレジスタ名とする
int get_var_addr(const char* var){
  int i;
  DebugOut2("regi count is %d\n", regi_var_count);
  for(i = 0; i < MAXADDR; i++){
    if(strcmp(vars[i].name, var) == 0)
      return vars[i].addr;
  }
  for(i = 0; i < regi_var_count; i++){
	  DebugOut2("regi %d is %s\n", i, regi_var[i].name);
	  if (strcmp(regi_var[i].name, var) == 0) {
		  return regi_var[i].addr;
	  }
  }
  fprintf(stderr, "var %s is not matching\n", var), exit(1);
  return -1;
}

// レジスタ番号を置き換える
// origin_regi: 変える対象のレジスタ名
// dest: これに変わる
void change_register_count(int target, char* dest){
  int i;
  char target_str[255];
  sprintf(target_str, "__r%d", target);

  for(i = 0; i <= node_count; i++){
    if(strcmp(target_str, nodes[i].l) == 0){
      strcpy(nodes[i].l, dest);
	  DebugOut2("taget:%s, dest:%s\n", target_str, dest);
    }

    if(strcmp(target_str, nodes[i].r) == 0){
      strcpy(nodes[i].r, dest);
	  DebugOut2("taget:%s, dest:%s\n", target_str, dest);
    }
  }
}

static void create_node(){
  for(int i = 0; i < nodes_replaced.size(); i++){
    for(int j = i+1; j < nodes_replaced.size(); j++){
      if(nodes_replaced[i].regi == nodes_replaced[j].regi){
        nodes_replaced[i].next = &nodes_replaced[j];
      }
      else {
        break;
      }
    }
  }
}

// 計算再利用のためのなんか
static void optimize_node(){
  int size = nodes_replaced.size();

  for(int i = 0; i < size; i++){
    for(int j = i+1; j < size; j++){
      if(nodes_replaced.at(i).op == nodes_replaced.at(j).op &&
        nodes_replaced.at(i).l == nodes_replaced.at(j).l &&
        nodes_replaced.at(i).r == nodes_replaced.at(j).r)
      {
        int tmp_i = i+1;
        int tmp_j = j+1;
        while(tmp_i < size && tmp_j < size &&
          nodes_replaced.at(tmp_i).r == nodes_replaced.at(tmp_j).r)
        {
          tmp_i++;
          tmp_j++;
        }

        int regi = nodes_replaced.at(j).regi;

        std::cout << tmp_i << " " << tmp_j << std::endl;
        nodes_replaced.erase(nodes_replaced.begin()+j, nodes_replaced.begin()+tmp_j);

        nodes_replaced.insert(nodes_replaced.begin()+(tmp_i),
          Node4cpp("L", "r"+std::to_string(regi),
                "r"+std::to_string(nodes_replaced.at(tmp_i-1).regi), 0));

        size = nodes_replaced.size();
      }
    }
  }
}

void output_suitable_load_word(const char* name, int regi_num){
  int i;
  SymData symbol;

  if(get_symbol(name, &symbol)){
    DebugOut2("serching success! %d, %d\n", symbol.addr, symbol.attr);
    if(symbol.attr == Arg){
      // Output2("load\tr%d, %d%s\n", regi_num, symbol.addr, get_additional_op[symbol.attr]);
      add_op("load", "r"+std::to_string(regi_num), std::to_string(symbol.addr)+get_additional_op(symbol.attr));
    }
    else{
      // Output2("load\tr%d, %d%s\n", regi_num, symbol.addr, get_additional_op[symbol.attr]);
      add_op("load", "r"+std::to_string(regi_num), std::to_string(symbol.addr) + get_additional_op(symbol.attr));
    }
  }
  else{
    DebugOut("serching register field");
    // それでもなかったらレジスタ変数
    for(i = 0; i < regi_var_count; i++){
      if(strcmp(regi_var[i].name, name) == 0)
        DebugOut("ばか");
    }
  }
}

int register_state[4];

int get_unused_register(void){
  int i;
  for(i = 0; i < 4; i++){
    if(register_state[i] == 0)
      return i;
  }
  return 0;
}


void maesyori(void) {
	int i;
	int regi;
	for (i = 0; i <= node_count; i++) {
		if (isRegister(nodes[i].l) ) {
			regi = atoi(&nodes[i].l[3]); // __r〇ってなってるはず
			change_register_count(nodes[i].regi, nodes[i].l);
			nodes[i].regi = regi;
		}
		else if (isRegister(nodes[i].r)) {
//			regi = atoi(&nodes[i].r[3]); // __r〇ってなってるはず
//			change_register_count(nodes[i].regi, nodes[i].r);
//			nodes[i].regi = regi;
		}
	}

	for (i = 0; i <= node_count; i++) {
		DebugOut2("(### op:%s l:%s r:%s regi:%d ###)\n", nodes[i].op, nodes[i].l, nodes[i].r, nodes[i].regi);
	}
}

void replace_nodes(){
  for(int i = 0; i <= node_count; ++i){
    nodes_replaced.emplace_back(nodes[i].op, nodes[i].l, nodes[i].r, nodes[i].regi);
  }
}

void hoge(const char* _operator, const char* _operand, int is_left, int _register) {
	int const_num, addr, register_;

	register_ = get_unused_register();

	if (isDigit(_operand)){
		const_num = atoi(_operand);
		if (const_num < -32768 || 32768 < const_num) {
			sprintf(data_labels[label_counter].name, "label%d: data %d", label_counter, const_num);

      // Output2("%s\tr%d, label%d\n", _operator,
			// 	is_left ? register_ : _register,
			// 	label_counter);
			// label_counter++;
      add_op(_operator,
        "r"+std::to_string(is_left ? register_ : _register),
        "label"+std::to_string(label_counter++));
		}
		else {
			// Output2("%si\tr%d, %d\n", _operator,
			// 	is_left ? register_ : _register,
			// 	const_num);
      add_op(std::string(_operator)+"i",
       "r"+std::to_string(is_left ? register_ : _register),
        std::to_string(const_num));
			register_state[register_] = 1;
		}
	}
	else {
		DebugOut("こっち");
    if(get_symbol(_operand, &symbol)){
      DebugOut2("serching success! %d, %d\n", symbol.addr, symbol.attr);
        // Output2("%s\tr%d, %d%s\n", _operator, is_left ? register_ : _register,
        // symbol.addr, get_additional_op[symbol.attr]);
      add_op(_operator, "r"+std::to_string(is_left?register_:_register),
        std::to_string(symbol.addr)+get_additional_op(symbol.attr));
    }
		else if (isRegister(_operand))
		{
			// Output2("%sr\tr%d, r%d\n", _operator,
			// 	_register, atoi(&_operand[3]));
      add_op(std::string(_operator)+"r", "r"+std::to_string(_register),
        "r"+std::to_string(atoi(&_operand[3])));
		}
		else {
      addr = get_var_addr(_operand);
			// Output2("%s\tr%d, %d\n", _operator,
			// 	is_left ? register_ : _register,
			// 	addr);
      add_op(_operator, "r"+std::to_string(is_left?register_:_register), std::to_string(addr));
		}
		register_state[register_] = 1;
	}

	//if (isDigit(nodes[i].r)) {
	//	const_num = atoi(nodes[i].r);
	//	if (const_num < -32768 || 32768 < const_num) {
	//		sprintf(data_labels[label_counter].name, "label%d: data %d", label_counter, const_num);

	//		Output2("add\tr0, label%d\n", label_counter);
	//		label_counter++;
	//	}
	//	else
	//		Output2("addi\tr0,%d\n", atoi(nodes[i].r));
	//}
	//else
	//{
	//	addr = get_var_addr(nodes[i].r);
	//	Output2("add\tr0,%d\n", addr);
	//}
}

int translate(std::vector<p1::Tetrad>& tetrads){
  int i, j, k, left_var_flag;
  int addr;
  int register_ = 0;
  int const_num;
  memset(register_state, 0, sizeof(int) * 4);

  nodes_replaced.clear();

  // 式が１つしかない場合
  if(strcmp(nodes[0].op, "") == 0){
    if(isDigit(nodes[0].l)){
      // ここで数値が16bitで扱えるかチェックして、ダメならラベルに登録
      const_num = atoi(nodes[0].l);
      if(const_num < -32768 || 32768 < const_num){
        auto label = Program::get_instance().push_const_label(const_num);

        // Output2("load\tr0, label%d\n", label_counter);
        add_op("load", "r0", "label"+std::to_string(label_counter));
        label_counter++;
      }
      // 普通の定数値
      else{
        // Output2("loadi\tr0, %d\n", const_num);
        add_op("loadi", "r0", std::to_string(const_num));
      }
    }
    else{
        output_suitable_load_word(nodes[0].l, 0);
//      Output2("load\tr0, %d\n", get_var_addr(nodes[0].l));
    }
    return 0;
  }


  DebugOut("before change");
  maesyori();
  DebugOut("after change");

  replace_nodes();

  create_node();

  optimize_node();

  for (i = 0; i < nodes_replaced.size(); i++) {
//	  DebugOut2("op:%s l:%s r:%s\n", nodes_replaced[i].op, nodes_replaced[i].l, nodes_replaced[i].r);
	  // 左にレジスタがあれば
	  if (isRegister(nodes_replaced[i].l.c_str()))
	  {
      register_ = nodes_replaced[i].regi;
		  if ((nodes_replaced[i].op == "+")) {
			  hoge("add", nodes_replaced[i].r.c_str(), 0, nodes_replaced[i].regi);
		  }
		  else if ((nodes_replaced[i].op == "-")) {
			  hoge("sub", nodes_replaced[i].r.c_str(), 0, nodes_replaced[i].regi);
		  }
		  else if ((nodes_replaced[i].op == "*")) {
			  hoge("mul", nodes_replaced[i].r.c_str(), 0, nodes_replaced[i].regi);
		  }
		  else if ((nodes_replaced[i].op == "div")) {
			  hoge("div", nodes_replaced[i].r.c_str(), 0, nodes_replaced[i].regi);
		  }
		  continue;
	  }

    // "L" 命令が入っていたら
    if(nodes_replaced.at(i).op == "L"){
      add_op("loadr", nodes_replaced.at(i).l, nodes_replaced.at(i).r);
      continue;
    }


	  // 左辺を出力
	  hoge("load", nodes_replaced[i].l.c_str(), 0, nodes_replaced[i].regi);
    register_ = nodes_replaced[i].regi;

	  if ((nodes_replaced[i].op == "+")) {
		  hoge("add", nodes_replaced[i].r.c_str(), 0, nodes_replaced[i].regi);
	  }
	  else if ((nodes_replaced[i].op ==  "-")) {
		  hoge("sub", nodes_replaced[i].r.c_str(), 0, nodes_replaced[i].regi);
	  }
	  else if ((nodes_replaced[i].op == "*")) {
		  hoge("mul", nodes_replaced[i].r.c_str(), 0, nodes_replaced[i].regi);
	  }
	  else if ((nodes_replaced[i].op == "div")) {
		  hoge("div", nodes_replaced[i].r.c_str(), 0, nodes_replaced[i].regi);
	  }

//	  Output2("store\tr0, %d\n", var_count + nodes[i].regi);
  }

  for (k = 0; k < 4; k++)
	  register_state[k] = 0;
//  Output2("load\tr0, %d\n", var_count + nodes[i-1].regi);

  DebugOut("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH");

  return register_;//atoi(&nodes[i-1].regi);//nodes[i-1].regi;
}
