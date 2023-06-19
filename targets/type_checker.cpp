#include <string>
#include "targets/type_checker.h"
#include ".auto/all_nodes.h" // automatically generated
#include <cdk/types/primitive_type.h>
#include "mml_parser.tab.h"

#define ASSERT_UNSPEC                                                 \
  {                                                                   \
    if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) \
      return;                                                         \
  }

//---------------------------------------------------------------------------

void mml::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl)
{
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_nil_node(cdk::nil_node *const node, int lvl)
{
  // EMPTY
}
void mml::type_checker::do_data_node(cdk::data_node *const node, int lvl)
{
  // EMPTY
}
void mml::type_checker::do_double_node(cdk::double_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
}
void mml::type_checker::do_not_node(cdk::not_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_INT))
  {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else if (node->argument()->is_typed(cdk::TYPE_UNSPEC))
  {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else
  {
    throw std::string("wrong type in unary logical expression");
  }
}
void mml::type_checker::do_and_node(cdk::and_node *const node, int lvl)
{
  do_BooleanLogicalExpression(node, lvl);
}
void mml::type_checker::do_or_node(cdk::or_node *const node, int lvl)
{
  do_BooleanLogicalExpression(node, lvl);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_integer_node(cdk::integer_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void mml::type_checker::do_string_node(cdk::string_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

//---------------------------------------------------------------------------

void mml::type_checker::do_neg_node(cdk::neg_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT))
    throw std::string("wrong type in argument of unary expression");

  // in MML, expressions are always int
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

// protected:
void mml::type_checker::do_PIDExpression(cdk::binary_operation_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE))
  {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
  {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE))
  {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT))
  {
    node->type(node->left()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER))
  {
    node->type(node->right()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT))
  {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC))
  {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else
  {
    throw std::string("wrong types in binary expression");
  }
}

void mml::type_checker::do_IDExpression(cdk::binary_operation_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE))
  {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
  {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE))
  {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT))
  {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC))
  {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else
  {
    throw std::string("wrong types in binary expression");
  }
}

void mml::type_checker::do_IntOnlyExpression(cdk::binary_operation_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT))
  {
    throw std::string("integer expression expected in binary operator (left)");
  }

  node->right()->accept(this, lvl + 2);
  if (!node->right()->is_typed(cdk::TYPE_INT))
  {
    throw std::string("integer expression expected in binary operator (right)");
  }

  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

// protected:
void mml::type_checker::do_ScalarLogicalExpression(cdk::binary_operation_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT) && !node->left()->is_typed(cdk::TYPE_DOUBLE))
  {
    throw std::string("integer expression expected in binary logical expression (left)");
  }

  node->right()->accept(this, lvl + 2);
  if (!node->right()->is_typed(cdk::TYPE_INT) && !node->right()->is_typed(cdk::TYPE_DOUBLE))
  {
    throw std::string("integer expression expected in binary logical expression (right)");
  }

  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

// protected:
void mml::type_checker::do_BooleanLogicalExpression(cdk::binary_operation_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT))
  {
    throw std::string("integer expression expected in binary expression");
  }

  node->right()->accept(this, lvl + 2);
  if (!node->right()->is_typed(cdk::TYPE_INT))
  {
    throw std::string("integer expression expected in binary expression");
  }

  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

// protected:
void mml::type_checker::do_GeneralLogicalExpression(cdk::binary_operation_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if (node->left()->type() != node->right()->type())
  {
    throw std::string("same type expected on both sides of equality operator");
  }
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void mml::type_checker::do_add_node(cdk::add_node *const node, int lvl)
{
  do_PIDExpression(node, lvl);
}
void mml::type_checker::do_sub_node(cdk::sub_node *const node, int lvl)
{
  do_PIDExpression(node, lvl);
}
void mml::type_checker::do_mul_node(cdk::mul_node *const node, int lvl)
{
  do_IDExpression(node, lvl);
}
void mml::type_checker::do_div_node(cdk::div_node *const node, int lvl)
{
  do_IDExpression(node, lvl);
}
void mml::type_checker::do_mod_node(cdk::mod_node *const node, int lvl)
{
  do_IntOnlyExpression(node, lvl);
}
void mml::type_checker::do_lt_node(cdk::lt_node *const node, int lvl)
{
  do_ScalarLogicalExpression(node, lvl);
}
void mml::type_checker::do_le_node(cdk::le_node *const node, int lvl)
{
  do_ScalarLogicalExpression(node, lvl);
}
void mml::type_checker::do_ge_node(cdk::ge_node *const node, int lvl)
{
  do_ScalarLogicalExpression(node, lvl);
}
void mml::type_checker::do_gt_node(cdk::gt_node *const node, int lvl)
{
  do_ScalarLogicalExpression(node, lvl);
}
void mml::type_checker::do_ne_node(cdk::ne_node *const node, int lvl)
{
  do_GeneralLogicalExpression(node, lvl);
}
void mml::type_checker::do_eq_node(cdk::eq_node *const node, int lvl)
{
  do_GeneralLogicalExpression(node, lvl);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_variable_node(cdk::variable_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<mml::symbol> symbol = _symtab.find(id);
  _parent->set_last_id(id);
  if (symbol != nullptr)
  {
    node->type(symbol->type());
  }
  else
  {
    throw id;
  }
}

void mml::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  try
  {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  }
  catch (const std::string &id)
  {
    throw "undeclared variable '" + id + "'";
  }
}

void mml::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 4);
  node->rvalue()->accept(this, lvl + 4);
  if (node->lvalue()->is_typed(cdk::TYPE_INT))
  {
    if (node->rvalue()->is_typed(cdk::TYPE_INT))
    {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    }
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC))
    {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    }
    else
    {
      throw std::string("wrong assignment to integer");
    }
  }
  else if (node->lvalue()->is_typed(cdk::TYPE_POINTER))
  {
    if (node->rvalue()->is_typed(cdk::TYPE_POINTER))
    {
      node->type(node->rvalue()->type());
    }
    else if (node->rvalue()->is_typed(cdk::TYPE_INT))
    {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_POINTER));
    }
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC))
    {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_ERROR));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_ERROR));
    }
    else
    {
      throw std::string("wrong assignment to pointer");
    }
  }
  else if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE))
  {

    if (node->rvalue()->is_typed(cdk::TYPE_DOUBLE) || node->rvalue()->is_typed(cdk::TYPE_INT))
    {
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    }
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC))
    {
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
      node->rvalue()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    }
    else
    {
      throw std::string("wrong assignment to real");
    }
  }
  else if (node->lvalue()->is_typed(cdk::TYPE_STRING))
  {

    if (node->rvalue()->is_typed(cdk::TYPE_STRING))
    {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
    }
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC))
    {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
    }
    else
    {
      throw std::string("wrong assignment to string");
    }
  }
  else if (node->lvalue()->is_typed(cdk::TYPE_FUNCTIONAL) && node->rvalue()->is_typed(cdk::TYPE_FUNCTIONAL))
  {

    if (node->lvalue()->type() == node->rvalue()->type())
    {
      node->type(node->lvalue()->type());
    }
    else
    {
      throw std::string("wrong assignment to string");
    }
  }
  else
  {
    throw std::string("wrong types in assignment");
  }
}

//---------------------------------------------------------------------------

void mml::type_checker::do_program_node(mml::program_node *const node, int lvl)
{
  node->statements()->accept(this, lvl + 2);
}

void mml::type_checker::do_evaluation_node(mml::evaluation_node *const node, int lvl)
{
  node->argument()->accept(this, lvl + 2);
}

void mml::type_checker::do_print_node(mml::print_node *const node, int lvl)
{
  node->arguments()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_input_node(mml::input_node *const node, int lvl)
{
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void mml::type_checker::do_while_node(mml::while_node *const node, int lvl)
{
  node->condition()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_if_node(mml::if_node *const node, int lvl)
{
  node->condition()->accept(this, lvl + 4);
  if (!node->condition()->is_typed(cdk::TYPE_INT))
    throw std::string("expected integer condition");
}

void mml::type_checker::do_if_else_node(mml::if_else_node *const node, int lvl)
{
  node->condition()->accept(this, lvl + 4);
  if (!node->condition()->is_typed(cdk::TYPE_INT))
    throw std::string("expected integer condition");
}

void mml::type_checker::do_address_of_node(mml::address_of_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 2);
  node->type(cdk::reference_type::create(4, node->lvalue()->type()));
}

void mml::type_checker::do_block_node(mml::block_node *const node, int lvl)
{
  node->declarations()->accept(this, lvl);
  node->instructions()->accept(this, lvl);
}

void mml::type_checker::do_function_call_node(mml::function_call_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  if (node->identifier() == nullptr)
  {

    if (_parent->get_last_type_size() == 0)
    {
      throw std::string("Recursive call outside of function");
    }
    auto symbol = _parent->get_last_type_group();
    std::shared_ptr<cdk::functional_type> type = std::dynamic_pointer_cast<cdk::functional_type>(symbol.type());
    node->type(type->output(0));
    return;
  }
  node->identifier()->accept(this, lvl);
  if (!node->identifier()->is_typed(cdk::TYPE_FUNCTIONAL))
  {
    throw std::string("identifier isn't a function addr");
  }
  std::shared_ptr<cdk::functional_type> type = std::dynamic_pointer_cast<cdk::functional_type>(node->identifier()->type());
  node->type(type->output(0));
  if (type->input_length() == node->arguments()->size())
  {
    node->arguments()->accept(this, lvl + 4);
    for (size_t ax = 0; ax < node->arguments()->size(); ax++)
    {
      if (node->argument(ax)->type() == type->input(ax))
        continue;
      if (type->input(ax)->name() == cdk::TYPE_DOUBLE && node->argument(ax)->is_typed(cdk::TYPE_INT))
        continue;
      throw std::string("type mismatch for argument " + std::to_string(ax + 1) + "'.");
    }
  }
  else
  {
    throw std::string(
        "number of arguments in call (" + std::to_string(node->arguments()->size()) + ") must match declaration (" + std::to_string(type->input_length()) + ").");
  }
}

void mml::type_checker::do_function_node(mml::function_node *const node, int lvl)
{
  _functionRets.push_back(node->type());
  _symtab.push();
  std::vector<std::shared_ptr<cdk::basic_type>> argtypes;
  std::vector<std::shared_ptr<cdk::basic_type>> returnType;
  for (size_t ax = 0; ax < node->arguments()->size(); ax++)
  {
    argtypes.push_back(node->argument(ax)->type());
    node->argument(ax)->accept(this, lvl);
  }
  returnType.push_back(node->type());
  node->block()->accept(this, lvl);
  _symtab.pop();
  _functionRets.pop_back();
  node->type(cdk::functional_type::create(argtypes, returnType));
}

void mml::type_checker::do_identity_node(mml::identity_node *const node, int lvl)
{
  // NOTHING YET
}

void mml::type_checker::do_index_node(mml::index_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  std::shared_ptr<cdk::reference_type> btype;

  if (node->base())
  {
    node->base()->accept(this, lvl + 2);
    btype = cdk::reference_type::cast(node->base()->type());
    if (!node->base()->is_typed(cdk::TYPE_POINTER))
      throw std::string("pointer expression expected in index left-value");
  }
  else
  {
    btype = cdk::reference_type::cast(_function->type());
    if (!_function->is_typed(cdk::TYPE_POINTER))
      throw std::string("return pointer expression expected in index left-value");
  }

  node->index()->accept(this, lvl + 2);
  if (!node->index()->is_typed(cdk::TYPE_INT))
    throw std::string("integer expression expected in left-value index");

  node->type(btype->referenced());
}

void mml::type_checker::do_next_node(mml::next_node *const node, int lvl)
{
  // NOTHING YET
}

void mml::type_checker::do_null_node(mml::null_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->type(cdk::reference_type::create(4, nullptr));
}

void mml::type_checker::do_return_node(mml::return_node *const node, int lvl)
{
  node->value()->accept(this, lvl + 2);

  std::shared_ptr<cdk::basic_type> _functionLast = _functionRets.at(_functionRets.size() - 1);


  if (_functionLast->name() == cdk::TYPE_INT)
  {
    if (!node->value()->is_typed(cdk::TYPE_INT))
      throw std::string("wrong type for initializer (integer expected).");
  }
  else if (_functionLast->name() == cdk::TYPE_DOUBLE)
  {
    if (!node->value()->is_typed(cdk::TYPE_INT) && !node->value()->is_typed(cdk::TYPE_DOUBLE))
    {
      throw std::string("wrong type for initializer (integer or double expected).");
    }
  }
  else if (_functionLast->name() == cdk::TYPE_STRING)
  {
    if (!node->value()->is_typed(cdk::TYPE_STRING))
    {
      throw std::string("wrong type for initializer (string expected).");
    }
  }
  else if (_functionLast->name() == cdk::TYPE_POINTER)
  {
    if (!node->value()->is_typed(cdk::TYPE_POINTER))
    {
      throw std::string("wrong type for initializer (pointer expected).");
    }
  }
  else if (_functionLast->name() == cdk::TYPE_VOID)
  {
    if (node->value())
    {
      throw std::string("no return value expected");
    }
  }
  else if (_functionLast->name() == cdk::TYPE_FUNCTIONAL)
  {
    if (!node->value()->is_typed(cdk::TYPE_FUNCTIONAL))
    {
      throw std::string("wrong type for initializer (function type expected).");
    }
  }
  else
  {
    throw std::string("unknown type for initializer.");
  }
}

void mml::type_checker::do_sizeof_node(mml::sizeof_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->expression()->accept(this, lvl + 2);
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void mml::type_checker::do_stack_alloc_node(mml::stack_alloc_node *const node, int lvl)
{
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT))
  {
    throw std::string("integer expression expected in allocation expression");
  }
  auto mytype = cdk::reference_type::create(4, cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  node->type(mytype);
}

void mml::type_checker::do_stop_node(mml::stop_node *const node, int lvl)
{
  // NOTHING YET
}

void mml::type_checker::do_variable_declaration_node(mml::variable_declaration_node *const node, int lvl)
{
  if (node->initializer() != nullptr)
  {
    node->initializer()->accept(this, lvl + 2);

    if (node->is_typed(cdk::TYPE_INT))
    {
      if (!node->initializer()->is_typed(cdk::TYPE_INT))
        throw std::string("wrong type for initializer (integer expected).");
    }
    else if (node->is_typed(cdk::TYPE_DOUBLE))
    {
      if (!node->initializer()->is_typed(cdk::TYPE_INT) && !node->initializer()->is_typed(cdk::TYPE_DOUBLE))
      {
        throw std::string("wrong type for initializer (integer or double expected).");
      }
    }
    else if (node->is_typed(cdk::TYPE_STRING))
    {
      if (!node->initializer()->is_typed(cdk::TYPE_STRING))
      {
        throw std::string("wrong type for initializer (string expected).");
      }
    }
    else if (node->is_typed(cdk::TYPE_POINTER))
    {
      // DAVID: FIXME: trouble!!!
      if (!node->initializer()->is_typed(cdk::TYPE_POINTER))
      {
        auto in = (cdk::literal_node<int> *)node->initializer();
        if (in == nullptr || in->value() != 0)
          throw std::string("wrong type for initializer (pointer expected).");
      }
    }
    else if (node->is_typed(cdk::TYPE_FUNCTIONAL))
    {
      if (node->initializer()->is_typed(cdk::TYPE_FUNCTIONAL))
      {
        if (node->type() != node->initializer()->type())
        {
          throw std::string("wrong functional type.");
        }
      }
    }
    else if (node->is_typed(cdk::TYPE_UNSPEC))
    {
      node->type(node->initializer()->type());
    }
    else
    {
      throw std::string("unknown type for initializer.");
    }
  }
  bool isFunc = false;
  if (node->qualifier() == tFORWARD || node->qualifier() == tFOREIGN)
  {
    if (node->initializer())
    {
      throw std::string("forward can't have initializer.");
    }
  }
  if (node->is_typed(cdk::TYPE_FUNCTIONAL))
  {
    isFunc = true;
  }
  const std::string &id = node->identifier();
  auto symbol = _symtab.find(id);
  if (symbol)
  {
    if (symbol->qualifier() == tFORWARD && node->qualifier() != tFORWARD)
    {
      symbol->set_qual(node->qualifier());
      _parent->set_new_symbol(symbol);
      return;
    }
  }
  symbol = mml::make_symbol(false, node->qualifier(), node->type(), id, (bool)node->initializer(), isFunc);
  if (_symtab.insert(id, symbol))
  {
    _parent->set_new_symbol(symbol); // advise parent that a symbol has been inserted
  }
  else
  {
    throw std::string("variable '" + id + "' redeclared");
  }
}