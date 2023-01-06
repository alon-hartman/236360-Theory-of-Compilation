%{
    #include "source.hpp"
    #include "hw3_output.hpp"
	#include "parser.tab.hpp"
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
void                                { yylval = new VoidType(yytext, yylineno); return VOID;}
int                                 { yylval = new IntType(yytext, yylineno); return INT; }
byte                                { yylval = new ByteType(yytext, yylineno); return BYTE; }
b                                   { yylval = new ByteType(yytext, yylineno); return B; }
bool                                { yylval = new BoolType(yytext, yylineno); return BOOL; }
and                                 { yylval = new Node(yytext, yylineno); return AND; }
or                                  { yylval = new Node(yytext, yylineno); return OR; }
not                                 { yylval = new Node(yytext, yylineno); return NOT; }
true                                { yylval = new Node(yytext, yylineno); return TRUE; }
false                               { yylval = new Node(yytext, yylineno); return FALSE; }
return                              { yylval = new Node(yytext, yylineno); return RETURN; }
if                                  { yylval = new Node(yytext, yylineno); return IF; }
else                                { yylval = new Node(yytext, yylineno); return ELSE; }
while                               { yylval = new Node(yytext, yylineno); return WHILE; }
break                               { yylval = new Node(yytext, yylineno); return BREAK; }
continue                            { yylval = new Node(yytext, yylineno); return CONTINUE; }
;                                   { yylval = new Node(yytext, yylineno); return SC; }
,                                   { yylval = new Node(yytext, yylineno); return COMMA; }
\(                                  { yylval = new Node(yytext, yylineno); return LPAREN; } 
\)                                  { yylval = new Node(yytext, yylineno); return RPAREN; }
\{                                  { yylval = new Node(yytext, yylineno); return LBRACE; }
\}                                  { yylval = new Node(yytext, yylineno); return RBRACE; }
=                                   { yylval = new Node(yytext, yylineno); return ASSIGN; }
{equalUnequal}				        { yylval = new Node(yytext, yylineno); return EQUALITY; }
{inequality}				        { yylval = new Node(yytext, yylineno); return RELATIONAL; }
{plusMinus}					        { yylval = new Node(yytext, yylineno); return ADDITIVE; }
{multDiv}					        { yylval = new Node(yytext, yylineno); return MULTIPLICATIVE; }
[a-zA-Z][a-zA-Z0-9]*                { yylval = new IDNode(yytext, yylineno); return ID; }
0|[1-9][0-9]*                       { yylval = new Num(yytext, yylineno); return NUM; }
\"([^\n\r\"\\]|\\[rnt"\\])+\"       { yylval = new StringType(yytext, yylineno); return STRING; }
{whitespace}                        { /* ignore */ };
{line_comment}                      { /* ignore */ };
.                                   { errorLex(yylineno); exit(0); };
