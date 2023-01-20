#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "bp.hpp"
#include "allocator.h"
#include "source.hpp"
#include <iostream>

std::string emitOperator(int val1, int val2, const char *op, const char *type = "i32");

void emitBinopRes(Node *lhs, Node *rhs, string op, Node *res);

void emitBooleanBlockRelaEq(Node *lhs, Node *rhs, string op, Node *res);

void emitBooleanBlockShortCircuit(Node *lhs, Node *rhs, string op, Node *res);

void emitLabelAndGoto(Node *res, LabelList &list);

void emitGoto(LabelList &list);

std::string openFunctionStack(Node *retType, Node *id, Node *formals);

void storeValInStack(std::string stack_reg, std::string val, unsigned int offset, Node *in = nullptr);

std::string loadValFromStack(std::string stack_reg, int offset, SymTable::Entry *in = nullptr);

void emitExpGoto(Node *node);

void emitTypeConversion(Node *lhs, Node *rhs);

void emitPrintFunctions();

std::string emitStringDeclarationAndAssignment(Node *node);

void emitFunctionCall(Node *func, SymTable::Entry *entry);

void emitFunctionReturn(Node *node, Node *return_val = nullptr);

void emitBooleanNoOp(Node *id_node);

void debug();

#endif
