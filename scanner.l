%{

//$Id: scanner.l,v 1.1 2015/06/04 06:18:20 mchaboll Exp $
// Marcos Chabolla
// mchaboll@ucsc.edu
// ID: 1437530
#include "auxlib.h"
#include "lyutils.h"

#define YY_USER_ACTION  { lexer_useraction (); }
#define IGNORE(THING)   { }

%}

%option 8bit
%option debug
%option ecs
%option nodefault
%option nounput
%option noyywrap
%option perf-report
%option verbose
%option warn

LETTER      [A-Za-z_]
DIGIT       [0-9]
NUMBER      ({DIGIT}+)
STRINGCON   (\"([^\\"\n]|\\[\\'"0nt])*\")
CHARCON     ('([^\\'\n]|\\[\\'"0nt])')

IDENT       ({LETTER}({LETTER}|{DIGIT})*)

%%

"#".*       { lexer_include(); }
"//".*      { IGNORE (comments) }
[ \t]+      { IGNORE (white space) }
\n          { lexer_newline(); }
"void"      { return yylval_token(TOK_VOID); }
"bool"      { return yylval_token(TOK_BOOL); }
"char"      { return yylval_token(TOK_CHAR); }
"int"       { return yylval_token(TOK_INT); }
"string"    { return yylval_token(TOK_STRING); }
"struct"    { return yylval_token(TOK_STRUCT); }
"if"        { return yylval_token(TOK_IF); }
"else"      { return yylval_token(TOK_ELSE); }
"while"     { return yylval_token(TOK_WHILE); }
"return"    { return yylval_token(TOK_RETURN); }
"false"     { return yylval_token(TOK_FALSE); }
"true"      { return yylval_token(TOK_TRUE); }
"null"      { return yylval_token(TOK_NULL); }
"ord"       { return yylval_token(TOK_ORD); }
"chr"       { return yylval_token(TOK_CHR); }
"new"       { return yylval_token(TOK_NEW); }
"[]"        { return yylval_token (TOK_ARRAY); }
"("         { return yylval_token ('('); }
")"         { return yylval_token (')'); }
"["         { return yylval_token ('['); }
"]"         { return yylval_token (']'); }
"{"         { return yylval_token ('{'); }
"}"         { return yylval_token ('}'); }
";"         { return yylval_token (';'); }
","         { return yylval_token (','); }
"."         { return yylval_token ('.'); }
"="         { return yylval_token ('='); }
"=="        { return yylval_token (TOK_EQ); }
"!="        { return yylval_token (TOK_NE); }
"<"         { return yylval_token (TOK_LT); }
"<="        { return yylval_token (TOK_LE); }
">"         { return yylval_token (TOK_GT); }
">="        { return yylval_token (TOK_GE); }
"+"         { return yylval_token ('+'); }
"-"         { return yylval_token ('-'); }
"*"         { return yylval_token ('*'); }
"/"         { return yylval_token ('/'); }
"%"         { return yylval_token ('%'); }
"!"         { return yylval_token ('!'); }


{CHARCON}   { return yylval_token (TOK_CHARCON); }
{STRINGCON} { return yylval_token (TOK_STRINGCON); }
{IDENT}     { return yylval_token (TOK_IDENT); }
{NUMBER}    { return yylval_token (TOK_INTCON); }

.    { lexer_badtoken (yytext); }

%%

