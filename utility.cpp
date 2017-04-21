#include "utility.h"

namespace util{
  bool isDigit(const std::string& s){
    int i = 0;
    //終端文字か数字以外の文字が出てくるまでループ
    for (i = 0; s[i] != NULL && isdigit(s[i]); ++i);
    //ループ終了時、終端に達していなければエラー
    return (s[i] == NULL);
  }

  bool isRegister(const std::string& s){
    return s.find("__") != std::string::npos;
  }

  bool isOperator(const std::string& s){
    if(s == "+"   ||
       s == "-"   ||
       s == "*"   ||
       s == "div" ||
       s == "!"   ||
       s == "("   ||
       s == ")"   ||
       s == "$"){
     return true;
   }
   else{
     return false;
   }
  }
}
