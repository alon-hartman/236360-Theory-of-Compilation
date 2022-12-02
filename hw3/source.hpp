#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>

enum class types
{
  None,
  Int,
  Bool,
  Byte,
  String,
  Func,
  Void
};

class Node
{
public:
  Node(char *yytext = "") : m_type(types::None), m_name(yytext) {}
  types m_type;
  std::string m_name;
  int m_num_val;
  std::string m_string_val;

  void setValue(Node &other)
  {
    if (other.m_type == types::String)
    {
      m_string_val = other.m_string_val;
      return;
    }
    m_num_val = other.m_num_val;
  }
  void SetValue(std::string string_val)
  {
    m_string_val = string_val;
  }
};

class Num : public Node
{
  Num(char *yytext) : Node(yytext), m_num_val(std::stoi(yytext)) {}
};

class IntType : public Node
{
  IntType(char *yytext = "") : Node(yytext) { m_type = types::Int; }
};
class VoidType : public Node
{
  VoidType(char *yytext = "") : Node(yytext) { m_type = types::Void; }
};
class ByteType : public Node
{
  ByteType(char *yytext = "") : Node(yytext) { m_type = types::Byte; }
};
class BoolType : public Node
{
  BoolType(char *yytext = "") : Node(yytext) { m_type = types::Bool; }
};
class StringType : public Node
{
  StringType(char *yytext = "") : Node(yytext)
  {
    m_type = types::String;
    m_string_val = yytext;
  }
};

#define YYSTYPE Node *

#endif