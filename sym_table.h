#ifndef SYM_TABLE_H
#define SYM_TABLE_H

#include <getsym.h>
#include <vector>
#include <string>

typedef enum SymAttr{
  Var, Local, Arg, Func
}SymAttr;

typedef struct SymData{
  char name[255];
  int addr;
  SymAttr attr;
}SymData;
#define MAXADDR 65535

/**@brief 記号表に追加
*@param[in] name 記号名
*@param[in] attr 記号の種類 (Var: グローバル変数, Local: ローカル変数, Arg: 引数, Func: 関数)
*/
void add_symbol(const char* name, SymAttr attr);

/**@brief 記号表からnameのつくデータを取得する
*@param[in] name 記号名
*@parem[out] data 記号名が一致したデータ
*@return 取得成功時: 1 失敗時: 0
*/
int get_symbol(const char* name, SymData* data);

/**@brief ローカル変数(と引数)の記号表を初期化する
*@note 関数定義が終了したらこれを呼ぶ
*/
void clear_local_vars(std::vector<SymData>* p_symlist = nullptr);

// for debug
void flush_all_symbols(void);

#endif
