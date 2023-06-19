#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/frame_size_calculator.h"
#include "targets/postfix_writer.h"
#include ".auto/all_nodes.h" // all_nodes.h is automatically generated
#include "mml_parser.tab.h"

//---------------------------------------------------------------------------

void mml::postfix_writer::do_nil_node(cdk::nil_node *const node, int lvl)
{
  // EMPTY
}
void mml::postfix_writer::do_data_node(cdk::data_node *const node, int lvl)
{
  // EMPTY
}
void mml::postfix_writer::do_double_node(cdk::double_node *const node, int lvl)
{
  if (_inFunctionBody || _inMain)
  {
    _pf.DOUBLE(node->value()); // load number to the stack
  }
  else
  {
    _pf.SDOUBLE(node->value()); // double is on the DATA segment
  }
}
void mml::postfix_writer::do_not_node(cdk::not_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl + 2);
  _pf.INT(0);
  _pf.EQ();
}
void mml::postfix_writer::do_and_node(cdk::and_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JZ(mklbl(lbl));
  node->right()->accept(this, lvl + 2);
  _pf.AND();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}
void mml::postfix_writer::do_or_node(cdk::or_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JNZ(mklbl(lbl));
  node->right()->accept(this, lvl + 2);
  _pf.OR();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_sequence_node(cdk::sequence_node *const node, int lvl)
{
  for (size_t i = 0; i < node->size(); i++)
  {
    node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_integer_node(cdk::integer_node *const node, int lvl)
{
  if (_inFunctionBody || _inMain)
  {
    _pf.INT(node->value()); // integer literal is on the stack: push an integer
  }
  else
  {
    _pf.SINT(node->value()); // integer literal is on the DATA segment
  }
}

void mml::postfix_writer::do_string_node(cdk::string_node *const node, int lvl)
{
  int lbl1;
  /* generate the string literal */
  _pf.RODATA();                    // strings are readonly DATA
  _pf.ALIGN();                     // make sure the address is aligned
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
  _pf.SSTRING(node->value());      // output string characters
  if (_inFunctionBody || _inMain)
  {
    // local variable initializer
    _pf.TEXT();
    _pf.ADDR(mklbl(lbl1));
  }
  else
  {
    // global variable initializer
    _pf.DATA();
    _pf.SADDR(mklbl(lbl1));
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_neg_node(cdk::neg_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.NEG();                           // 2-complement
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_add_node(cdk::add_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT)
  {
    _pf.I2D();
  }
  else if (node->type()->name() == cdk::TYPE_POINTER && node->left()->type()->name() == cdk::TYPE_INT)
  {
    _pf.INT(3);
    _pf.SHTL();
  }

  node->right()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT)
  {
    _pf.I2D();
  }
  else if (node->type()->name() == cdk::TYPE_POINTER && node->right()->type()->name() == cdk::TYPE_INT)
  {
    _pf.INT(3);
    _pf.SHTL();
  }

  if (node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DADD();
  else
    _pf.ADD();
}

void mml::postfix_writer::do_sub_node(cdk::sub_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT)
  {
    _pf.I2D();
  }
  else if (node->type()->name() == cdk::TYPE_POINTER && node->right()->type()->name() == cdk::TYPE_INT)
  {
    _pf.INT(3);
    _pf.SHTL();
  }

  if (node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DSUB();
  else
    _pf.SUB();
}

void mml::postfix_writer::do_mul_node(cdk::mul_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT)
    _pf.I2D();

  if (node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DMUL();
  else
    _pf.MUL();
}
void mml::postfix_writer::do_div_node(cdk::div_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT)
    _pf.I2D();

  if (node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DDIV();
  else
    _pf.DIV();
}
void mml::postfix_writer::do_mod_node(cdk::mod_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}
void mml::postfix_writer::do_lt_node(cdk::lt_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.I2D();

  _pf.LT();
}
void mml::postfix_writer::do_le_node(cdk::le_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.I2D();

  _pf.LE();
}
void mml::postfix_writer::do_ge_node(cdk::ge_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.I2D();

  _pf.GE();
}
void mml::postfix_writer::do_gt_node(cdk::gt_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.I2D();

  _pf.GT();
}
void mml::postfix_writer::do_ne_node(cdk::ne_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.I2D();

  _pf.NE();
}
void mml::postfix_writer::do_eq_node(cdk::eq_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.I2D();

  _pf.EQ();
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_variable_node(cdk::variable_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;

  const std::string &id = node->name();
  auto symbol = _symtab.find(id);
  if (symbol->global())
  {
    _pf.ADDR(symbol->name());
  }
  else
  {
    _pf.LOCAL(symbol->offset());
    // std::cerr << "LVAL " << symbol->name() << ":" << symbol->type()->size() << ":" << symbol->offset() << std::endl;
  }
}

void mml::postfix_writer::do_rvalue_node(cdk::rvalue_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
  if (node->type()->name() == cdk::TYPE_DOUBLE)
  {
    _pf.LDDOUBLE();
  }
  else
  {
    // integers, pointers, and strings
    _pf.LDINT();
  }
}

void mml::postfix_writer::do_assignment_node(cdk::assignment_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;

  node->rvalue()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE)
  {
    if (node->rvalue()->type()->name() == cdk::TYPE_INT)
      _pf.I2D();
    _pf.DUP64();
  }
  else
  {
    _pf.DUP32();
  }

  node->lvalue()->accept(this, lvl);
  if (node->type()->name() == cdk::TYPE_DOUBLE)
  {
    _pf.STDOUBLE();
  }
  else
  {
    _pf.STINT();
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_program_node(mml::program_node *const node, int lvl)
{
  // Note that MML doesn't have functions. Thus, it doesn't need
  // a function node. However, it must start in the main function.
  // The ProgramNode (representing the whole program) doubles as a
  // main function node.

  // generate the main function (RTS mandates that its name be "_main")
  _pf.TEXT();
  _pf.ALIGN();
  _pf.GLOBAL("_main", _pf.FUNC());
  _pf.LABEL("_main");
  _symtab.push();
  frame_size_calculator lsc(_compiler, _symtab);
  node->accept(&lsc, lvl + 2);
  _symtab.pop();
  _pf.ENTER(lsc.localsize());
  _inMain = true;
  node->statements()->accept(this, lvl + 2);
  _inMain = false;
  // end the main function
  if (!_returnSeen)
  {
    _pf.INT(0);
    _pf.STFVAL32();
  }
  _pf.LEAVE();
  _pf.RET();

  // these are just a few library function imports
  while (!_functions_to_declare.empty())
  {
    std::set<std::string>::iterator iter = _functions_to_declare.begin();
    _pf.EXTERN(*iter);
    _functions_to_declare.erase(iter);
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_evaluation_node(mml::evaluation_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  _pf.TRASH(node->argument()->type()->size());
}

void mml::postfix_writer::do_print_node(mml::print_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;

  for (size_t ix = 0; ix < node->arguments()->size(); ix++)
  {
    auto child = dynamic_cast<cdk::expression_node *>(node->arguments()->node(ix));

    std::shared_ptr<cdk::basic_type> etype = child->type();
    child->accept(this, lvl); // expression to print
    if (etype->name() == cdk::TYPE_INT)
    {
      _functions_to_declare.insert("printi");
      _pf.CALL("printi");
      _pf.TRASH(4); // trash int
    }
    else if (etype->name() == cdk::TYPE_DOUBLE)
    {
      _functions_to_declare.insert("printd");
      _pf.CALL("printd");
      _pf.TRASH(8); // trash double
    }
    else if (etype->name() == cdk::TYPE_STRING)
    {
      _functions_to_declare.insert("prints");
      _pf.CALL("prints");
      _pf.TRASH(4); // trash char pointer
    }
    else
    {
      std::cerr << "cannot print expression of unknown type" << std::endl;
      return;
    }
  }

  if (node->newline())
  {
    _functions_to_declare.insert("println");
    _pf.CALL("println");
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_input_node(mml::input_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _functions_to_declare.insert("readd");
    _pf.CALL("readd");
    _pf.LDFVAL64();
  } else if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_UNSPEC)) {
    _functions_to_declare.insert("readi");
    _pf.CALL("readi");
    _pf.LDFVAL32();
  } else {
    std::cerr << "FATAL: " << node->lineno() << ": cannot read type" << std::endl;
    return;
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_while_node(mml::while_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  _pf.LABEL(mklbl(lbl1 = ++_lbl));
  _loop_cond.push_back(mklbl(lbl1));
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl2 = ++_lbl));
  _loop_end.push_back(mklbl(lbl2));
  node->block()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl1));
  _pf.LABEL(mklbl(lbl2));
  _loop_cond.pop_back();
  _loop_end.pop_back();
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_if_node(mml::if_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_if_else_node(mml::if_else_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

void mml::postfix_writer::do_address_of_node(mml::address_of_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  // since the argument is an lvalue, it is already an address
  node->lvalue()->accept(this, lvl + 2);
}

void mml::postfix_writer::do_block_node(mml::block_node *const node, int lvl)
{
  _symtab.push(); // for block-local vars
  if (node->declarations())
    node->declarations()->accept(this, lvl + 2);
  if (node->instructions())
    node->instructions()->accept(this, lvl + 2);
  _symtab.pop();
}

void mml::postfix_writer::do_function_call_node(mml::function_call_node *const node, int lvl)
{
  size_t argsSize = 0;
  std::shared_ptr<cdk::functional_type> type;
  std::string id = "";
  if (node->identifier() != nullptr)
  {
    ASSERT_SAFE_EXPRESSIONS;
    type = std::dynamic_pointer_cast<cdk::functional_type>(node->identifier()->type());
    id = last_id();
  }
  else
  {
    auto __symbol = _type_group.at(_type_group.size() - 1);
    type = std::dynamic_pointer_cast<cdk::functional_type>(__symbol.type());
    id = __symbol.identifier();
  }
  if (node->arguments()->size() > 0)
  {
    for (int ax = node->arguments()->size() - 1; ax >= 0; ax--)
    {
      cdk::expression_node *arg = dynamic_cast<cdk::expression_node *>(node->arguments()->node(ax));
      arg->accept(this, lvl + 2);
      if (type->input(ax)->name() == cdk::TYPE_DOUBLE && arg->is_typed(cdk::TYPE_INT))
      {
        _pf.I2D();
      }
      argsSize += type->input(ax)->size();
    }
  }

  if (node->identifier() != nullptr)
  {
    if (id != "")
    {
      auto symbol = _symtab.find(id);
      if (symbol && symbol->qualifier() == tFOREIGN)
      {
        _pf.CALL(id);
      }
      else
      {
        node->identifier()->accept(this, lvl);
        _pf.BRANCH();
      }
    }
    else
    {
      node->identifier()->accept(this, lvl);
      _pf.BRANCH();
    }
    reset_last_id();
  }
  else
  {
    _pf.CALL(id);
  }

  if (argsSize != 0)
  {
    _pf.TRASH(argsSize);
  }

  if (type->output(0)->name() == cdk::TYPE_INT || type->output(0)->name() == cdk::TYPE_POINTER || type->output(0)->name() == cdk::TYPE_STRING || type->output(0)->name() == cdk::TYPE_FUNCTIONAL)
  {
    _pf.LDFVAL32();
  }
  else if (type->output(0)->name() == cdk::TYPE_DOUBLE)
  {
    _pf.LDFVAL64();
  }
  else
  {
    // cannot happer!
  }
}

void mml::postfix_writer::do_function_node(mml::function_node *const node, int lvl)
{
  std::shared_ptr<cdk::basic_type> retType = node->type();
  _functions.push_back((cdk::functional_type::cast(node->type()))->output(0));
  if (_inFunctionArgs)
  {
    error(node->lineno(), "cannot define function in arguments");
    return;
  }
  int endLabel = ++_lbl;
  if (_inMain || _inFunctionBody)
  {
    _pf.JMP(mklbl(endLabel));
  }
  _funcLabels.push_back(mklbl(++_lbl));
  _address_group.push_back(_lbl);
  push_address_group(_lbl);
  _symtab.push();
  auto symbol = mml::make_symbol(false, tPRIVATE, node->type(), _funcLabels.at(_funcLabels.size() - 1), false, true);
  _type_group.push_back(*symbol);
  push_type_group(*symbol);

  _symtab.pop();
  _retLabels.push_back(mklbl(++_lbl));
  _offset = 8;
  _symtab.push();
  if (node->arguments()->size() > 0)
  {
    _inFunctionArgs = true;
    for (size_t ix = 0; ix < node->arguments()->size(); ix++)
    {
      cdk::basic_node *arg = node->arguments()->node(ix);
      if (arg == nullptr)
        break;              // this means an empty sequence of arguments
      arg->accept(this, 0); // the function symbol is at the top of the stack
    }
    _inFunctionArgs = false;
  }
  _pf.TEXT();
  _pf.ALIGN();
  _pf.GLOBAL(_funcLabels.at(_funcLabels.size() - 1), _pf.FUNC());
  _pf.LABEL(_funcLabels.at(_funcLabels.size() - 1));
  _symtab.push();
  frame_size_calculator lsc(_compiler, _symtab);
  node->accept(&lsc, lvl);
  _symtab.pop();
  _pf.ENTER(lsc.localsize());
  _offset = 0;

  _inFunctionBody = true;
  os() << "        ;; before body " << std::endl;
  node->block()->accept(this, lvl + 4); // block has its own scope
  _functions.pop_back();
  os() << "        ;; after body " << std::endl;
  _type_group.pop_back();
  _address_group.pop_back();
  pop_address_group();
  pop_type_group();
  if (_funcLabels.size() - 1 == 0)
  {
    _inFunctionBody = false;
  }
  _returnSeen = false;
  _pf.LABEL(_retLabels.at(_retLabels.size() - 1));
  _retLabels.pop_back();
  _pf.LEAVE();
  _pf.RET();
  if (_inMain || _inFunctionBody)
  {
    _pf.LABEL(mklbl(endLabel));
  }

  _symtab.pop();

  if (_inFunctionBody || _inMain)
  {
    _pf.ADDR(_funcLabels.at(_funcLabels.size() - 1));
  }
  else
  {
    _pf.DATA();
    _pf.SADDR(_funcLabels.at(_funcLabels.size() - 1));
  }
  _funcLabels.pop_back();
}

void mml::postfix_writer::do_identity_node(mml::identity_node *const node, int lvl)
{
  // NOTHING YET
}

void mml::postfix_writer::do_index_node(mml::index_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  if (node->base())
  {
    node->base()->accept(this, lvl);
  }
  else
  {
    if (_functions.size() > 0)
    {
      _pf.LOCV(-_functions.at(_functions.size() - 1)->size());
    }
    else
    {
      std::cerr << "FATAL: " << node->lineno() << ": trying to use return value outside function" << std::endl;
    }
  }
  node->index()->accept(this, lvl);
  _pf.INT(3);
  _pf.SHTL();
  _pf.ADD(); // add pointer and index
}

void mml::postfix_writer::do_next_node(mml::next_node *const node, int lvl)
{
  if (_loop_cond.size() != 0 && (int)(_loop_cond.size()) >= node->level())
  {
    _pf.JMP(_loop_cond.at(_loop_cond.size() - node->level())); // jump to next cycle
  }
  else
    error(node->lineno(), "'restart' outside 'for'");
}

void mml::postfix_writer::do_null_node(mml::null_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  if (_inFunctionBody || _inMain)
  {
    _pf.INT(0);
  }
  else
  {
    _pf.SINT(0);
  }
}

void mml::postfix_writer::do_return_node(mml::return_node *const node, int lvl)
{
  if (_inMain && !(_inFunctionBody))
  {
    node->value()->accept(this, lvl + 2);
    _pf.STFVAL32();
  }
  else
  {
    // should not reach here without returning a value (if not void)
    if (_functions.at(_functions.size() - 1)->name() != cdk::TYPE_VOID)
    {
      node->value()->accept(this, lvl + 2);
      std::shared_ptr<cdk::basic_type> type = _functions.at(_functions.size() - 1);

      if (type->name() == cdk::TYPE_INT || type->name() == cdk::TYPE_STRING || type->name() == cdk::TYPE_POINTER || type->name() == cdk::TYPE_FUNCTIONAL)
      {
        _pf.STFVAL32();
      }
      else if (type->name() == cdk::TYPE_DOUBLE)
      {
        if (node->value()->type()->name() == cdk::TYPE_INT)
          _pf.I2D();
        _pf.STFVAL64();
      }
      else
      {
        std::cerr << node->lineno() << ": should not happen: unknown return type" << std::endl;
      }
    }
  }
  if (_retLabels.size() != 0 && _inFunctionBody)
  {
    _pf.JMP(_retLabels.at(_retLabels.size() - 1));
  }
  _returnSeen = true;
}

void mml::postfix_writer::do_sizeof_node(mml::sizeof_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  _pf.INT(node->expression()->type()->size());
}

void mml::postfix_writer::do_stack_alloc_node(mml::stack_alloc_node *const node, int lvl)
{
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  _pf.INT(3);
  _pf.SHTL();
  _pf.ALLOC(); // allocate
  _pf.SP();    // put base pointer in stack
}

void mml::postfix_writer::do_stop_node(mml::stop_node *const node, int lvl)
{
  if (_loop_end.size() != 0 && (int)(_loop_end.size()) >= node->level())
  {
    _pf.JMP(_loop_end.at(_loop_end.size() - node->level())); // jump to next cycle
  }
  else
    error(node->lineno(), "'restart' outside 'for'");
}

void mml::postfix_writer::do_variable_declaration_node(mml::variable_declaration_node *const node, int lvl)
{

  ASSERT_SAFE_EXPRESSIONS;
  auto id = node->identifier();


  // type size?
  int offset = 0, typesize = node->type()->size(); // in bytes
  if (_inFunctionArgs)
  {
    offset = _offset;
    _offset += typesize;
  }
  else if (_inFunctionBody || _inMain)
  {
    _offset -= typesize;
    offset = _offset;
  }
  else
  {
    offset = 0; // global variable
  }

  auto symbol = new_symbol();

  if (node->is_typed(cdk::TYPE_UNSPEC))
  {
    std::cerr << "initializer type is undetermined" << std::endl;
  }

  if (node->is_typed(cdk::TYPE_FUNCTIONAL))
  {
    std::set<std::string>::iterator curr = _functions_to_declare.find(node->identifier());
    if (curr != _functions_to_declare.end())
    {
      _functions_to_declare.erase(curr);
    }
  }

  if (symbol)
  {
    symbol->set_offset(offset);
    reset_new_symbol();
  }
  if (node->qualifier() == tFORWARD || node->qualifier() == tFOREIGN)
  {
    _functions_to_declare.insert(node->identifier());
    return;
  }
  if (_inFunctionBody || _inMain || _inFunctionArgs)
  {
    if (node->initializer())
    {
      node->initializer()->accept(this, lvl);
      if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_STRING) || node->is_typed(cdk::TYPE_POINTER) || node->is_typed(cdk::TYPE_FUNCTIONAL))
      {
        _pf.LOCAL(symbol->offset());
        _pf.STINT();
      }
      else if (node->is_typed(cdk::TYPE_DOUBLE))
      {
        if (node->initializer()->is_typed(cdk::TYPE_INT))
          _pf.I2D();
        _pf.LOCAL(symbol->offset());
        _pf.STDOUBLE();
      }
      else
      {
        std::cerr << "cannot initialize" << std::endl;
      }
    }
  }
  else
  {
    if (!_inFunctionBody && !_inMain)
    {
      if (node->initializer() == nullptr)
      {
        _pf.BSS();
        _pf.ALIGN();
        _pf.LABEL(id);
        _pf.SALLOC(typesize);
      }
      else
      {

        if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_DOUBLE) || node->is_typed(cdk::TYPE_POINTER) || node->is_typed(cdk::TYPE_FUNCTIONAL))
        {
          _pf.DATA();
          _pf.ALIGN();
          _pf.LABEL(id);
          if (node->is_typed(cdk::TYPE_INT))
          {
            node->initializer()->accept(this, lvl);
          }
          else if (node->is_typed(cdk::TYPE_POINTER) || node->is_typed(cdk::TYPE_FUNCTIONAL))
          {
            node->initializer()->accept(this, lvl);
          }
          else if (node->is_typed(cdk::TYPE_DOUBLE))
          {
            if (node->initializer()->is_typed(cdk::TYPE_DOUBLE))
            {
              node->initializer()->accept(this, lvl);
            }
            else if (node->initializer()->is_typed(cdk::TYPE_INT))
            {
              cdk::integer_node *dclini = dynamic_cast<cdk::integer_node *>(node->initializer());
              cdk::double_node ddi(dclini->lineno(), dclini->value());
              ddi.accept(this, lvl);
            }
            else
            {
              std::cerr << node->lineno() << ": '" << id << "' has bad initializer for real value\n";
              _errors = true;
            }
          }
        }
        else if (node->is_typed(cdk::TYPE_STRING))
        {
          _pf.DATA();
          _pf.ALIGN();
          _pf.LABEL(id);
          node->initializer()->accept(this, lvl);
        }
        else
        {
          std::cerr << node->lineno() << ": '" << id << "' has unexpected initializer\n";
          _errors = true;
        }
      }
    }
  }
}
