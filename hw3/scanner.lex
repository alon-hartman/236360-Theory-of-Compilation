%{
	#include "parser.tab.hpp"
    #include "hw3_output.hpp"
    #include "source.hpp"
    using namespace output;
%}


%option yylineno
%option noyywrap

whitespace   ([\t\n\r ])
line_comment (\/\/[^\r\n]*[ \r|\n|\r\n]?)
multDiv         (\*|[/])
plusMinus       (\+|-)
equalUnequal    (==|!=)
inequality      (<|>|<=|>=)


%%
void                                { yylval = new VoidType(yytext); return Void;}
int                                 { yylval = new IntType(yytext); return INT; }
byte                                { yylval = new ByteType(yytext); return BYTE; }
b                                   { yylval = new ByteType(yytext); return B; }
bool                                { yylval = new BoolType(yytext); return BOOL; }
and                                 { yylval = new Node(yytext); return AND; }
or                                  { yylval = new Node(yytext); return OR; }
not                                 { yylval = new Node(yytext); return NOT; }
true                                { yylval = new Node(yytext); return TRUE; }
false                               { yylval = new Node(yytext); return FALSE; }
return                              { yylval = new Node(yytext); return RETURN; }
if                                  { yylval = new Node(yytext); return IF; }
else                                { yylval = new Node(yytext); return ELSE; }
while                               { yylval = new Node(yytext); return WHILE; }
break                               { yylval = new Node(yytext); return BREAK; }
continue                            { yylval = new Node(yytext); return CONTINUE; }
;                                   { yylval = new Node(yytext); return SC; }
,                                   { yylval = new Node(yytext); return COMMA; }
\(                                  { yylval = new Node(yytext); return LPAREN; }
\)                                  { yylval = new Node(yytext); return RPAREN; }
\{                                  { yylval = new Node(yytext); return LBRACE; }
\}                                  { yylval = new Node(yytext); return RBRACE; }
=                                   { yylval = new Node(yytext); return ASSIGN; }
{equalUnequal}				        { yylval = new Node(yytext); return EQUALITY; }
{inequality}				        { yylval = new Node(yytext); return RELATIONAL; }
{plusMinus}					        { yylval = new Node(yytext); return ADDITIVE; }
{multDiv}					        { yylval = new Node(yytext); return MULTIPLICATIVE; }
[a-zA-Z][a-zA-Z0-9]*                { yylval = new Node(yytext); return ID; }
0|[1-9][0-9]*                       { yylval = new Num(yytext); return NUM; }
\"([^\n\r\"\\]|\\[rnt"\\])+\"       { yylval = new String(yytext); return STRING; }
{whitespace}                        { /* ignore */ };
{line_comment}                      { /* ignore */ };
.                                   { errorLex(yylineno); exit(0); };
