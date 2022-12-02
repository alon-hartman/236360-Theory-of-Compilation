#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <cstdarg>
#include <cassert>

enum class types
{
  None,
  Int,
  Bool,
  Byte,
  String,
  Func,
  Void,
  Operator
};

struct Node
{
  Node(const char *yytext = "", int yylineno = -1) : m_type(types::None), m_name(yytext), m_lineno(yylineno) {}
  virtual ~Node() {}
  types m_type;
  std::string m_name;
  int m_num_val;
  std::string m_string_val;
  int m_lineno;

  std::vector<types> m_types_list;

  virtual void setValue(Node *other)
  {
    switch (other->m_type)
    {
    case types::Int:
    case types::Bool:
    case types::Byte:
      m_num_val = other->m_num_val;
      return;
    case types::String:
      m_string_val = other->m_string_val;
      return;
    default:
      return;
    }
  }
  virtual void setValue(Node *lhs, Node *op, Node *rhs)
  {
    assert(op->m_type == types::Operator);
    if (op->m_name == "==")
      this->m_num_val = (lhs->m_num_val == rhs->m_num_val);
    else if (op->m_name == "!=")
      this->m_num_val = (lhs->m_num_val != rhs->m_num_val);
    else if (op->m_name == "<")
      this->m_num_val = (lhs->m_num_val < rhs->m_num_val);
    else if (op->m_name == ">")
      this->m_num_val = (lhs->m_num_val > rhs->m_num_val);
    else if (op->m_name == "<=")
      this->m_num_val = (lhs->m_num_val <= rhs->m_num_val);
    else if (op->m_name == ">=")
      this->m_num_val = (lhs->m_num_val >= rhs->m_num_val);
    else if (op->m_name == "+")
      this->m_num_val = (lhs->m_num_val + rhs->m_num_val);
    else if (op->m_name == "-")
      this->m_num_val = (lhs->m_num_val - rhs->m_num_val);
    else if (op->m_name == "*")
      this->m_num_val = (lhs->m_num_val * rhs->m_num_val);
    else if (op->m_name == "/")
      this->m_num_val = (lhs->m_num_val / rhs->m_num_val);
  }

  virtual void setName(Node *other)
  {
    this->m_name = other->m_name;
  }
  virtual const char *getName()
  {
    return m_name.c_str();
  }
  virtual void setType(Node *other)
  {
    this->m_type = other->m_type;
  }
  void SetValue(std::string string_val)
  {
    m_string_val = string_val;
  }
};

struct Num : public Node
{
  Num(const char *yytext = "", int yylineno = -1) : Node(yytext, yylineno)
  {
    m_num_val = std::stoi(yytext);
  }
  ~Num() {}
};

struct OpNode : public Node
{
  OpNode(const char *yytext = "", int yylineno = -1) : Node(yytext, yylineno) { m_type = types::Operator; }
  ~OpNode() {}
};

struct IntType : public Node
{
  IntType(const char *yytext = "", int yylineno = -1) : Node(yytext, yylineno) { m_type = types::Int; }
  ~IntType() {}
};

struct VoidType : public Node
{
  VoidType(const char *yytext = "", int yylineno = -1) : Node(yytext, yylineno) { m_type = types::Void; }
  ~VoidType() {}
};

struct ByteType : public Node
{
  ByteType(const char *yytext = "", int yylineno = -1) : Node(yytext, yylineno) { m_type = types::Byte; }
  ~ByteType() {}
};

struct BoolType : public Node
{
  BoolType(const char *yytext = "", int yylineno = -1) : Node(yytext, yylineno) { m_type = types::Bool; }
  ~BoolType() {}
};

struct StringType : public Node
{
  StringType(const char *yytext = "", int yylineno = -1) : Node(yytext, yylineno)
  {
    m_type = types::String;
    m_string_val = yytext;
  }
  ~StringType() {}
};

struct IDNode : public Node
{
  IDNode(const char *yytext = "", int yylineno = -1) : Node(yytext, yylineno)
  {
    m_type = types::None;
    m_name = yytext;
  }
  ~IDNode() {}
};

void Delete(int count, ...)
{

  va_list list;

  va_start(list, count);

  for (int i = 0; i < count; i++)
  {
    delete va_arg(list, Node *);
  }

  va_end(list);
}

#define YYSTYPE Node *

#endif