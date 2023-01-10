#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "bp.hpp"
#include "allocator.h"
#include <iostream>

// forward declarations
class Node;

std::string emitOperator(int val1, int val2, const char *op, const char *type = "i32");

void emitBinopRes(Node *lhs, Node *rhs, string op, Node *res);

void emitBooleanBlockRelaEq(Node *lhs, Node *rhs, string op, Node *res);

void emitBooleanBlockShortCircuit(Node *lhs, Node *rhs, string op, Node *res);

void emitLabelAndGoto(Node *res, std::vector<std::pair<int, BranchLabelIndex>> &list);

std::string openFunctionStack(Node *retType, Node *id, Node *formals);

void storeValInStack(std::string stack_reg, std::string val, unsigned int offset);

std::string loadValFromStack(std::string stack_reg, unsigned int offset);

#endif
