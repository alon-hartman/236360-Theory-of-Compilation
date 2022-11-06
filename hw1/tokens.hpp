#ifndef TOKENS_HPP_
#define TOKENS_HPP_
// #include
enum tokentype
{
    VOID = 1,
    INT,
    BYTE,
    B,
    BOOL,
    AND,
    OR,
    NOT,
    TRUE,
    FALSE,
    RETURN,
    IF,
    ELSE,
    WHILE,
    BREAK,
    CONTINUE,
    SC,
    COMMA,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    ASSIGN,
    RELOP,
    BINOP,
    COMMENT,
    ID,
    NUM,
    STRING,
    ERROR,
    UNRECOGNIZED_CHAR,
    UNCLOSED_STRING,
    HEX_LENGTH_0,
    HEX_LENGTH_1,
    ESCAPE_ERROR,
    ENUM_SIZE
};
extern int yylineno;
extern char *yytext;
extern int yyleng;
extern int yylex();
#endif /* TOKENS_HPP_ */
