%{
	#include <stdio.h>
	#include "tokens.hpp"
	
%}


%option yylineno
%option noyywrap

digit 		([0-9])
hex			([0-9A-F])
letter 		([a-zA-Z])
whitespace  ([\t\n\r ])
id          {letter}({letter}|{digit})*

%%
void													return VOID;
int														return INT;
byte													return BYTE;
bool													return BOOL;
and														return AND;
or														return OR;
not														return NOT;
true													return TRUE;
false													return FALSE;
return													return RETURN;
if														return IF;
else													return ELSE;
while													return WHILE;
break													return BREAK;
continue												return CONTINUE;
b														return B;
;														return SC;
,														return COMMA;
\(														return LPAREN;
\)														return RPAREN;
\{														return LBRACE;
\}														return RBRACE;
=														return ASSIGN;
==|!=|<=|>=|<|>											return RELOP;
[\+\-\*\/]												return BINOP;
\/\/([^\n\r])+|\/\/										return COMMENT;
{id}													return ID;
0[1-9][0-9]*											return ERROR; /* fail on leading zero in number */
{digit}+\.{digit}+										return ERROR;  /* fail on float */
([1-9]{digit}*)|(0)										return NUM;
\"([^\"])*[\x0A\x0D]+.*\"|\"([^\"\n\r])*				return UNCLOSED_STRING;  /* fail on unclosed string */
(\"(.*)([^\\])((\\){2})*\\x.\")|(\"((\\){2})*\\x.\") 	return HEX_LENGTH_1;  /* fail on illegal hex character after \x */
(\".*[^\\]((\\){2})*\\x\")|(\"((\\){2})*\\x\") 			return HEX_LENGTH_0;  /* fail on \x */
(\"([^(\n)(\\)(\r)]*)([^\\])((\\){2})*\\[^nrxt0\\\"][^(\n)(\\)(\r)]*\")|(\"((\\){2})*\\[^nrxt0\\\"][^(\n)(\")(\\)(\r)]*\") 				return ESCAPE_ERROR;
\"([\x00-\x09\x0b-\x0c\x0e-\x21\x23-\x5b\x5d-\x7f]|((\\)(\\))|((\\)(\"))|((\\)(n))|((\\)(r))|((\\)(t))|((\\)(0))|((\\)x))*[^\"\n]*\"	return STRING; /* 0x9=TAB */
{whitespace}						{ /* ignore */ };
.														return UNRECOGNIZED_CHAR;
