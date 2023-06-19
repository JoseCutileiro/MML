%option c++ prefix="mml_scanner_" outfile="mml_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <cstring>
#include <sstream>
#include <cfloat>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "mml_parser.tab.h"

// output stream for building string literals
static std::ostringstream strlit;

#define INT_MAX 2147483647   // Maximum value for a 4-byte integer
#define DOUBLE_MAX DBL_MAX   // Maximum value for a 8-byte float
// don't change this
#define yyerror LexerError
%}
%x X_COMMENT
%x X_BACKSLASH
%x X_STRING

DIGIT                            [0-9]
EXPONENT                         [eE][-+]?{DIGIT}+
REAL                             {DIGIT}+{EXPONENT}|({DIGIT}+"."{DIGIT}|{DIGIT}"."{DIGIT}+){EXPONENT}?

%%
  yydebug=1; set_debug(1);

"//".*$                ;  /* match the whole line (it is ignored) */

"/*"                   yy_push_state(X_COMMENT);
<X_COMMENT>"*/"        yy_pop_state();
<X_COMMENT>.|"\n"      ;  /* ignore inside comments */

"auto"                   return tTYPE_AUTO;
"int"                    return tTYPE_INT;
"double"                 return tTYPE_DOUBLE;
"string"                 return tTYPE_STRING;
"void"                   return tTYPE_VOID;

"null"                   return tNULL;
"sizeof"                 return tSIZEOF;

"foreign"                return tFOREIGN;
"forward"                return tFORWARD;
"public"                 return tPUBLIC;

[\[\](),~]             return *yytext;
"->"                   return tSET; 
">="                   return tGE; 
"<="                   return tLE; 
"=="                   return tEQ; 
"!="                   return tNE;
"&&"                   return tAND;
"||"                   return tOR;

"return"               return tRETURN;
"!"                    return tPRINT; 
"!!"                   return tPRINTLN;
"input"                return tINPUT; 

"if"                   return tIF; 
"elif"                 return tELIF; 
"else"                 return tELSE; 

"while"                return tWHILE;
"next"                 return tNEXT;
"stop"                 return tSTOP;

"begin"                return tBEGIN; 
"end"                  return tEND; 

[A-Za-z][A-Za-z0-9_]*  yylval.s = new std::string(yytext); return tIDENTIFIER;

\"                   yy_push_state(X_STRING);
<X_STRING>\\           yy_push_state(X_BACKSLASH);
<X_STRING>\"         {
                          yylval.s = new std::string(strlit.str());
                          strlit.str("");
                          yy_pop_state();
                          return tSTRING;
                        }
<X_STRING>.            strlit << yytext;                        

<X_BACKSLASH>"n"       strlit << '\n'; yy_pop_state();
<X_BACKSLASH>"r"       strlit << '\r'; yy_pop_state();
<X_BACKSLASH>"t"       strlit << '\t'; yy_pop_state();
<X_BACKSLASH>"\""      strlit << '\"'; yy_pop_state();
<X_BACKSLASH>[[:xdigit:]]+ {
                                 strlit << (char)(unsigned char)strtoul(yytext, NULL, 8);
                                 yy_pop_state();
                               }
<X_BACKSLASH>.         strlit << *yytext; yy_pop_state();

{REAL}                  { double value = strtod(yytext, NULL);
                          if (value > DOUBLE_MAX) yyerror("Double overflow");
                          yylval.d = value; return tDOUBLE;
                        }


0[0-7]+              yylval.i = strtol(yytext, nullptr, 8); return tOCTAL;
0[0-9]+              yyerror("Invalid octal number");
[0-9]+               { long long int value = strtol(yytext, nullptr, 10);
                      if (value > INT_MAX) yyerror("Integer overflow");
                      yylval.i = value; return tINTEGER;
                     }


[-()<>=+*/%;{}.?@]       return *yytext;

[ \t\n]+               ; /* ignore whitespace */

.                      yyerror("Unknown character");

%%

// TODO BACKSLASH