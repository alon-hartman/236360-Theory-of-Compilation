%{
	#include "parser.tab.hpp"
    #include "output.hpp"
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
void                                return VOID;
int                                 return INT;
byte                                return BYTE;
b                                   return B;
bool                                return BOOL;
and                                 return AND;
or                                  return OR;
not                                 return NOT;
true                                return TRUE;
false                               return FALSE;
return                              return RETURN;
if                                  return IF;
else                                return ELSE;
while                               return WHILE;
break                               return BREAK;
continue                            return CONTINUE;
;                                   return SC;
,                                   return COMMA;
\(                                  return LPAREN;
\)                                  return RPAREN;
\{                                  return LBRACE;
\}                                  return RBRACE;
=                                   return ASSIGN;
{equalUnequal}				        return EQUALITY;
{inequality}				        return RELATIONAL;
{plusMinus}					        return ADDITIVE;
{multDiv}					        return MULTIPLICATIVE;
[a-zA-Z][a-zA-Z0-9]*                return ID;
0|[1-9][0-9]*                       return NUM;
\"([^\n\r\"\\]|\\[rnt"\\])+\"       return STRING;
{whitespace}                        { /* ignore */ };
{line_comment}                      { /* ignore */ };
.                                   { errorLex(yylineno); exit(0); };
