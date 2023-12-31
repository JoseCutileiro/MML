#ifndef __MML_AST_FUNCTION_H__
#define __MML_AST_FUNCTION_H__

#include <cdk/ast/typed_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include "ast/block_node.h"

namespace mml
{

  class function_node : public cdk::expression_node
  {
    cdk::sequence_node *_arguments;
    mml::block_node *_block;


  public:
    function_node(int lineno, cdk::sequence_node *arguments, std::shared_ptr<cdk::basic_type> varType, mml::block_node *block)
          : cdk::expression_node(lineno), _arguments(arguments), _block(block)
    {
      type(varType);
    }

  public:
    cdk::sequence_node* arguments()
    {
      return _arguments;
    }

    cdk::typed_node* argument(size_t ax) {
      return dynamic_cast<cdk::typed_node*>(_arguments->node(ax));
    }

    mml::block_node* block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level)
    {
      sp->do_function_node(this, level);
    }
  };

} // mml

#endif
