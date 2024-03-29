#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <cstdarg>
#include <cassert>

#include "hw3_output.hpp"
#include "bp.hpp"
using namespace output;
// forward declaration
struct Node;

using LabelList = vector<pair<int, BranchLabelIndex>>;

enum class types
{
  None,
  Int,
  Bool,
  Byte,
  String,
  Void
};

const int BYTE_SIZE = 255;

class SymTable
{
  SymTable();
public:
  enum class scope_type
  {
    BLOCK,
    FUNC,
    IF,
    WHILE,
    ELSE
  };
  struct Entry
  {
    Entry(std::string name, types return_type, std::vector<types> types_vec, bool is_func, int offset): name(name), return_type(return_type), types_vec(types_vec), is_func(is_func), offset(offset) {}
    std::string name;
    types return_type;
    std::vector<types> types_vec;
    bool is_func;
    int offset;
  };
  struct Scope
  {
    std::vector<Entry> entries;
    int offset;
    int min_arg_offset;
    scope_type type;
    Scope(int offset): entries(), offset(offset), min_arg_offset(-1), type(scope_type::BLOCK) {}
  };
  std::vector<Scope> scopes_stack;
  bool has_main;
  std::string curr_func_ptr;

  static SymTable &getInstance();
  Scope &push(scope_type type = scope_type::BLOCK);
  void pop();
  Scope &top();
  void insert(Node *entry, bool is_func = false, Node *rhs = nullptr);

  void insert_arg(Entry entry);
  void insert_arg(Node *node);

  Entry *find_entry(const std::string &name);
  bool isInScope(SymTable::scope_type scope_type);
};

struct Node
{
  Node(const char *yytext = "", int yylineno = -1): m_type(types::Void), m_name(yytext), m_lineno(yylineno), m_reg(""), m_val(0) {}
  virtual ~Node() {}
  types m_type;
  string m_name;
  int m_lineno;
  string m_reg;
  int m_val;
  LabelList true_list;
  LabelList false_list;
  LabelList next_list;
  string m_label;

  vector<types> m_types_list;
  vector<string> m_reg_list;

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

  bool isNumType()
  {
    return ((m_type == types::Int) || (m_type == types::Byte));
  }

  virtual std::string getReg()
  {
    return m_reg;
  }
};

struct Num: public Node
{
  Num(const char *yytext = "", int yylineno = -1, bool eval = true): Node(yytext, yylineno)
  {
    m_type = types::Int;
    if (eval)
      m_val = std::stoi(m_name);
  }
  ~Num() {}
  std::string getReg() override;
};
struct IntType: public Node
{
  IntType(const char *yytext = "", int yylineno = -1): Node(yytext, yylineno) { m_type = types::Int; }
  ~IntType() {}
};

struct VoidType: public Node
{
  VoidType(const char *yytext = "", int yylineno = -1): Node(yytext, yylineno) { m_type = types::Void; }
  ~VoidType() {}
};

struct ByteType: public Num
{
  ByteType(const char *yytext = "", int yylineno = -1, bool eval = true): Num(yytext, yylineno, eval)
  {
    m_type = types::Byte;
    if (m_val > BYTE_SIZE) {
      errorByteTooLarge(yylineno, m_name);
      exit(0);
    }
  }
  ~ByteType() {}
  std::string getReg() override;
};

struct BoolType: public Node
{
  BoolType(const char *yytext = "", int yylineno = -1): Node(yytext, yylineno)
  {
    m_type = types::Bool;
    m_val = 0;
    if (m_name == "true")
      m_val = 1;
  }
  ~BoolType() {}
  std::string getReg() override;
};

struct StringType: public Node
{
  StringType(const char *yytext = "", int yylineno = -1): Node(yytext, yylineno)
  {
    m_type = types::String;
  }
  ~StringType() {}
};

struct IDNode: public Node
{
  IDNode(const char *yytext = "", int yylineno = -1): Node(yytext, yylineno)
  {
    m_type = types::None;
  }
  std::string getReg() override;
  ~IDNode() {}
};

void Delete(int count, ...);

std::string TypeToString(types type);
std::string TypeToIRString(types type);
std::vector<std::string> TypesToStrings(std::vector<types> &vec);
std::string TypesToIRString(std::vector<types> &vec);
bool allowed_implicit_assignment(types lhs, types rhs);
bool allowed_explicit_assignment(types lhs, Node *node);
void check_args_type(Node *node, SymTable::Entry *entry);

#define YYSTYPE Node *

#endif