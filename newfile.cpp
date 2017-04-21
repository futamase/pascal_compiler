#include "newfile.h"
#include <getsym.h>
#include <string>
#include <unordered_map>
#include "output.h"

extern TOKEN tok;
namespace {
  static std::string hoge(){
    //  "+"
    //  "-"
    //  "*"
    // "div"
  }
}

namespace p1
{
  Assignment::Assignment()
  {
     m_type = (Statement::Type::Assignment);
  }

  Assignment::Assignment(const std::string& lhs)
      : m_lhs(lhs)
  {
     m_type = (Statement::Type::Assignment);
     m_rhs.read();
  }

  void Assignment::dump(){
    auto tetrads = m_rhs.getTetrads();

    for(auto&& i : tetrads){
      std::cout << i.opcode << " " << i.op_r << " " << i.op_l << " " << i.dest << std::endl;
      Commander::get_instance().add_command({i.opcode, i.op_l, i.op_r});
    }

//    bool is_found = false;
//    if(m_caller){
//      is_found = m_caller->find_var()
//    }
//    else {
//
//    }

  }

  If::If()
  {
    m_type = (Statement::Type::If);
  }

  While::While()
  {
    m_type = (Statement::Type::While);
  }

  CalledProcedure::CalledProcedure()
  {
    m_type = (Statement::Type::CalledProcedure);
  }

  Write::Write()
  {
    m_type = (Statement::Type::Write);
  }

  void Write::read(){
    do{
      getsym();
      if(tok.attr == IDENTIFIER)
        m_vars.emplace_back(tok.charvalue);

      getsym();
      if(tok.attr == SYMBOL && tok.value == COMMA)
        continue;
      else
        break;
    }while(true);
  }
}
