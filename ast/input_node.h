#ifndef __MML_AST_INPUT_H__
#define __MML_AST_INPUT_H__

#include <cdk/ast/expression_node.h>

namespace mml
{

  class input_node : public cdk::expression_node
  {
  public:
    input_node(int lineno) : cdk::expression_node(lineno)
    {
    }

  public:
    void accept(basic_ast_visitor *sp, int level)
    {
      sp->do_input_node(this, level);
    }
  };

} // mml

#endif
