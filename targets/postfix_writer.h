#ifndef __MML_TARGETS_POSTFIX_WRITER_H__
#define __MML_TARGETS_POSTFIX_WRITER_H__

#include "targets/basic_ast_visitor.h"
#include <set>
#include <vector>
#include <sstream>
#include <cdk/emitters/basic_postfix_emitter.h>

namespace mml
{

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer : public basic_ast_visitor
  {
    cdk::symbol_table<mml::symbol> &_symtab;
    std::set<std::string> _functions_to_declare;
    std::vector<std::string> _loop_cond;
    std::vector<std::string> _loop_end;
    bool _errors, _inFunctionBody, _inFunctionArgs;
    bool _inMain;
    bool _returnSeen;                       // when building a function
    std::vector<std::shared_ptr<cdk::basic_type>> _functions; // for keeping track of the current function and its arguments
    int _offset;                            // current framepointer offset (0 means no vars defined)
    cdk::basic_postfix_emitter &_pf;
    int _lbl;
    std::vector<int> _address_group;
    std::vector<mml::symbol> _type_group;
    std::vector<std::string> _retLabels;
    std::vector<std::string> _funcLabels;

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<mml::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) : basic_ast_visitor(compiler), _symtab(symtab), _errors(false), _inFunctionBody(false), _inFunctionArgs(false), _inMain(false), _returnSeen(false), _offset(0), _pf(pf), _lbl(0)
    {
    }

  public:
    ~postfix_writer()
    {
      os().flush();
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl)
    {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

    void error(int lineno, std::string s)
    {
      std::cerr << "error: " << lineno << ": " << s << std::endl;
    }

  public:
    // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h" // automatically generated
#undef __IN_VISITOR_HEADER__
    // do not edit these lines: end
  };

} // mml

#endif
