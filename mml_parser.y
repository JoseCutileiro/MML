%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include ".auto/all_nodes.h"
#define LINE                         compiler->scanner()->lineno()
#define yylex()                      compiler->scanner()->scan()
#define yyerror(compiler, s)         compiler->scanner()->error(s)
//-- don't change *any* of these --- END!
%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expression type */
  //-- don't change *any* of these --- END!

  int                   i;	/* integer value */
  double                d;	/* integer value */
  std::string          *s;	/* symbol name or string literal */
  cdk::basic_node      *node;	/* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  mml::block_node      *block;
  std::vector<std::shared_ptr<cdk::basic_type>> *types;
};

%token <i> tINTEGER tOCTAL
%token <d> tDOUBLE
%token <s> tIDENTIFIER tSTRING
%token <i> tPUBLIC tFOREIGN tFORWARD tPRIVATE
%token tWHILE tIF tELIF tELSE tINPUT tBEGIN tEND
%token tPRINT tPRINTLN
%token tTYPE_STRING tTYPE_INT tTYPE_DOUBLE tTYPE_AUTO tTYPE_VOID
%token tOR tAND tNOT
%token tNULL tSIZEOF
%token tGE tLE tEQ tNE tSET
%token tNEXT tSTOP tRETURN

%type<i> qual
%type<s> string
%type<node> instruction return program iffalse
%type<sequence> file instructions stmt exprs 
%type<expression> expr opt_initializer funcdef
%type<block> block
%type<node> decl arg 
%type<lvalue> lvalue
%type<type> type functype
%type<types> types vector_type
%type<sequence> decls args

%nonassoc tIF LVALUE EMPTY
%nonassoc tELIF tELSE EXPR
%right '='
%left tAND tOR
%right '~'
%left tEQ tNE
%left '<' '>' tLE tGE
%left '+' '-'
%left '*' '/' '%'
%left tSET
%right tUNARY
%left '(' 
%right ')'


%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%
file : decls program      { compiler->ast($$ = new cdk::sequence_node(LINE, $2, $1));}
     | program            { compiler->ast($$ = new cdk::sequence_node(LINE, $1));}
     | decls              { compiler->ast($$ = $1); }
     |                    { compiler->ast($$ = new cdk::sequence_node(LINE));}
     ;

decls: decl              { $$ = new cdk::sequence_node(LINE, $1);}
     | decls decl        { $$ = new cdk::sequence_node(LINE, $2, $1);}
     ;

decl : type tIDENTIFIER opt_initializer               { $$ = new mml::variable_declaration_node(LINE, tPRIVATE, $1, *$2, $3); }
     | qual type tIDENTIFIER opt_initializer          { $$ = new mml::variable_declaration_node(LINE, $1, $2, *$3, $4); }
     | tTYPE_AUTO tIDENTIFIER '=' expr ';'            { $$ = new mml::variable_declaration_node(LINE, tPRIVATE, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC), *$2, $4); }
     | qual tIDENTIFIER '=' expr ';'                  { $$ = new mml::variable_declaration_node(LINE, $1, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC), *$2, $4); }
     | qual tTYPE_AUTO tIDENTIFIER '=' expr ';'       { $$ = new mml::variable_declaration_node(LINE, $1, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC), *$3, $5); }
     ;

program	: tBEGIN stmt tEND { $$ = new mml::program_node(LINE, $2); }
	      ;

stmt : decls instructions { $$ = new cdk::sequence_node(LINE, $2, $1);}
     | instructions       { $$ = new cdk::sequence_node(LINE, $1);}
     | decls              { $$ = new cdk::sequence_node(LINE, $1);}
     |                    { $$ = new cdk::sequence_node(LINE);}
    ;

instructions : instruction                   { $$ = new cdk::sequence_node(LINE, $1);}
             | instructions instruction      { $$ = new cdk::sequence_node(LINE, $2, $1);}
             ;

instruction :  tIF '(' expr ')' instruction %prec tIF            { $$ = new mml::if_node(LINE, $3, $5); }
            |  tIF '(' expr ')' instruction iffalse %prec tELSE  { $$ = new mml::if_else_node(LINE, $3, $5, $6);}
            |  tWHILE '(' expr ')' instruction       { $$ = new mml::while_node(LINE, $3, $5); }
            |  block                                 { $$ = $1; }
            |  tNEXT ';'                             { $$ = new mml::next_node(LINE); }
            |  tNEXT tINTEGER ';'                    { $$ = new mml::next_node(LINE, $2); }
            |  tSTOP ';'                             { $$ = new mml::stop_node(LINE); }
            |  tSTOP tINTEGER ';'                    { $$ = new mml::stop_node(LINE, $2); }
            |  expr ';'                              { $$ = new mml::evaluation_node(LINE, $1); }
            |  exprs tPRINT                          { $$ = new mml::print_node(LINE, $1, false); }
            |  exprs tPRINTLN                        { $$ = new mml::print_node(LINE, $1, true); }
            |  return                                { $$ = $1; }
            ;

iffalse : tELIF '(' expr ')' instruction %prec tIF             { $$ = new mml::if_node(LINE, $3, $5);}
        | tELIF '(' expr ')' instruction iffalse %prec tELSE   { $$ = new mml::if_else_node(LINE, $3, $5, $6);}
        | tELSE instruction                        { $$ = $2;}
        ; 

exprs: exprs ',' expr          { $$ = new cdk::sequence_node(LINE, $3, $1); }
     | expr                    { $$ = new cdk::sequence_node(LINE, $1); }
     ;

expr : tINTEGER                    { $$ = new cdk::integer_node(LINE, $1); }
     | tOCTAL                      { $$ = new cdk::integer_node(LINE, $1); }
     | tDOUBLE                     { $$ = new cdk::double_node(LINE, $1); }
     | tNULL                       { $$ = new mml::null_node(LINE); }
     | tINPUT                      { $$ = new mml::input_node(LINE); }
     | string                      { $$ = new cdk::string_node(LINE, $1); }
     | lvalue %prec EMPTY          { $$ = new cdk::rvalue_node(LINE, $1); }
     | lvalue '=' expr             { $$ = new cdk::assignment_node(LINE, $1, $3); }
     | expr '+' expr               { $$ = new cdk::add_node(LINE, $1, $3); }
     | expr '-' expr               { $$ = new cdk::sub_node(LINE, $1, $3); }
     | expr '*' expr               { $$ = new cdk::mul_node(LINE, $1, $3); }
     | expr '/' expr               { $$ = new cdk::div_node(LINE, $1, $3); }
     | expr '%' expr               { $$ = new cdk::mod_node(LINE, $1, $3); }
     | expr '>' expr               { $$ = new cdk::gt_node(LINE, $1, $3); }
     | expr '<' expr               { $$ = new cdk::lt_node(LINE, $1, $3); }
     | expr tGE expr               { $$ = new cdk::ge_node(LINE, $1, $3); }
     | expr tLE expr               { $$ = new cdk::le_node(LINE, $1, $3); }
     | expr tEQ expr               { $$ = new cdk::eq_node(LINE, $1, $3); }
     | expr tNE expr               { $$ = new cdk::ne_node(LINE, $1, $3); }
     | expr tAND expr              { $$ = new cdk::and_node(LINE, $1, $3); }
     | expr tOR expr               { $$ = new cdk::or_node(LINE, $1, $3); }
     | '+' expr %prec tUNARY       { $$ = $2;}
     | '-' expr %prec tUNARY       { $$ = new cdk::neg_node(LINE, $2);}
     | '~' expr                    { $$ = new cdk::not_node(LINE, $2); }
     | '(' expr ')'                { $$ = $2; }
     | tSIZEOF '(' expr ')'        { $$ = new mml::sizeof_node(LINE, $3); }
     | funcdef                     { $$ = $1; }
     | expr '(' exprs ')'          { $$ = new mml::function_call_node(LINE, $1, $3); }
     | lvalue '(' exprs ')' %prec EXPR        { $$ = new mml::function_call_node(LINE, new cdk::rvalue_node(LINE, $1), $3); }
     | lvalue '(' ')'       %prec EXPR        { $$ = new mml::function_call_node(LINE, new cdk::rvalue_node(LINE, $1), new cdk::sequence_node(LINE)); }
     | '@' '(' exprs ')'           { $$ = new mml::function_call_node(LINE, nullptr, $3); }
     | '@' '(' ')'                 { $$ = new mml::function_call_node(LINE, nullptr, new cdk::sequence_node(LINE)); }
     | '[' expr ']'                { $$ = new mml::stack_alloc_node(LINE, $2); }
     | lvalue '?'                  { $$ = new mml::address_of_node(LINE, $1); }
     ;

funcdef : '(' ')' tSET type block { $$ = new mml::function_node(LINE, new cdk::sequence_node(LINE), $4, $5); }
        | '(' args ')' tSET type block { $$ = new mml::function_node(LINE, $2, $5, $6); }
        ;

args: args ',' arg            { $$ = new cdk::sequence_node(LINE, $3, $1); }
    | arg                     { $$ = new cdk::sequence_node(LINE, $1); }
    ;

arg: type tIDENTIFIER        {$$ = new mml::variable_declaration_node(LINE, $1, *$2, nullptr);}


block : '{' instructions '}' { $$ = new mml::block_node(LINE, new cdk::sequence_node(LINE), $2); }
      | '{' decls '}' { $$ = new mml::block_node(LINE, $2, new cdk::sequence_node(LINE)); }
      | '{' decls instructions '}' { $$ = new mml::block_node(LINE, $2, $3); }
      | '{' '}' { $$ = new mml::block_node(LINE, new cdk::sequence_node(LINE), new cdk::sequence_node(LINE)); }
      ;

opt_initializer  : /* empty */ ';'     { $$ = nullptr; }
                 | '=' expr ';'  { $$ = $2; }
                 ;

lvalue : tIDENTIFIER %prec LVALUE     { $$ = new cdk::variable_node(LINE, *$1); }
       | lvalue '[' expr ']'          { $$ = new mml::index_node(LINE, new cdk::rvalue_node(LINE, $1), $3); }
       | '(' expr ')' '[' expr ']'    { $$ = new mml::index_node(LINE, $2, $5); }
       ;

qual : tPUBLIC { $$ = tPUBLIC;}
     | tFOREIGN { $$ = tFOREIGN;}
     | tFORWARD { $$ = tFORWARD;}
     ;

type : tTYPE_INT         { $$ = cdk::primitive_type::create(4, cdk::TYPE_INT); }
     | tTYPE_DOUBLE      { $$ = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE); }
     | tTYPE_STRING      { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
     | tTYPE_VOID        { $$ = cdk::primitive_type::create(0, cdk::TYPE_VOID); }
     | '[' type ']'      { $$ = cdk::reference_type::create(4, $2); }
     | functype          { $$ = $1; }
     ;

functype  : vector_type '<' types '>'  { $$ = cdk::functional_type::create(*$3, *$1);}
          | vector_type '<' '>'  { $$ = cdk::functional_type::create(*(new std::vector<std::shared_ptr<cdk::basic_type>>()), *$1);}

vector_type : type       { $$ = new std::vector<std::shared_ptr<cdk::basic_type>>(); $$->push_back($1); }

types: types ',' type    { $$ = $1; $$->push_back($3); }
     | type              { $$ = new std::vector<std::shared_ptr<cdk::basic_type>>(); $$->push_back($1); }


string: tSTRING               { $$ = $1; }
      |string tSTRING         { $$ = $1; $$->append(*$2); delete $2; }
      ;

return: tRETURN ';'            { $$ = new mml::return_node(LINE, nullptr); }
      | tRETURN expr ';'       { $$ = new mml::return_node(LINE, $2); }
      ;


%%
