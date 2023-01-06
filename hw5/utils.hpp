#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "bp.hpp"
#include "source.hpp"
#include "allocator.h"
#include <iostream>

std::string emitOperator(int val1, int val2, const char *op, const char *type = "i32")
{
    std::string returnReg = Allocator::getInstance().fresh_var(op + std::to_string(val1) + "to" + std::to_string(val2));
    CodeBuffer::instance().emit(returnReg + " = " + op + " " + type + " " + std::to_string(val1) + ", " + std::to_string(val2));
    return returnReg;
}

std::string emitBinopRes(Node *lhs, Node *rhs, string op)
{
    Allocator &allocator = Allocator::getInstance();
    CodeBuffer &codebuffer = CodeBuffer::instance();
    std::string operationSize = "i8";
    std::string leftReg = lhs->m_reg;
    std::string rightReg = rhs->m_reg;
    std::string resultReg;
    if (lhs->m_type == types::Int || rhs->m_type == types::Int)
    {
        operationSize = "i32";
        if (lhs->m_type == types::Byte)
        {
            leftReg = allocator.fresh_var("leftReg8to32");
            codebuffer.emit(leftReg + " = zext i8 " + lhs->m_reg + " to i32");
        }
        if (rhs->m_type == types::Byte)
        {
            rightReg = allocator.fresh_var("rightReg8to32");
            codebuffer.emit(rightReg + " = zext i8 " + rhs->m_reg + " to i32");
        }
    }
    if (op == "/")
    {
        /**
         * TODO: need to emit a compare of rightReg with 0, and goto somewhere we print division error
         *       we call print in llvm.
         */
        if (operationSize == "i8")
        {
            resultReg = allocator.fresh_var("udivResult");
            codebuffer.emit(resultReg + " = udiv " + operationSize + " " + leftReg + ", " + rightReg);
        }
        else // operationSize == "i32"
        {
            resultReg = allocator.fresh_var("sdivResult");
            codebuffer.emit(resultReg + " = sdiv " + operationSize + " " + leftReg + ", " + rightReg);
        }
    }
    else if (op == "*")
    {
        resultReg = allocator.fresh_var("mulResult");
        codebuffer.emit(resultReg + " = mul " + operationSize + " " + leftReg + ", " + rightReg);
    }
    else if (op == "+")
    {
        resultReg = allocator.fresh_var("addResult");
        codebuffer.emit(resultReg + " = add " + operationSize + " " + leftReg + ", " + rightReg);
    }
    else if (op == "-")
    {
        resultReg = allocator.fresh_var("subResult");
        codebuffer.emit(resultReg + " = sub " + operationSize + " " + leftReg + ", " + rightReg);
    }
    else
    {
        std::cout << "should never reach here!\n";
        exit(0);
    }
    return resultReg;
}

void emitBooleanBlockRelaEq(Node *lhs, Node *rhs, string op, Node *res)
{
    CodeBuffer &cb = CodeBuffer::instance();
    Allocator &allocator = Allocator::getInstance();
    res->m_label = cb.genLabel();
    string opType;
    string opSize = "i8";
    string leftReg = lhs->m_reg;
    string rightReg = rhs->m_reg;
    if (lhs->m_type == types::Int || rhs->m_type == types::Int)
    {
        opSize = "i32";
        if (lhs->m_type == types::Byte)
        {
            leftReg = allocator.fresh_var("leftReg8to32");
            cb.emit(leftReg + " = zext i8 " + lhs->m_reg + " to i32");
        }
        if (rhs->m_type == types::Byte)
        {
            rightReg = allocator.fresh_var("rightReg8to32");
            cb.emit(rightReg + " = zext i8 " + rhs->m_reg + " to i32");
        }
    }
    res->m_reg = allocator.fresh_var("rela_" + lhs->m_name + "_" + rhs->m_name);
    if (op == "==")
        opType = "eq";
    else if (op == "!=")
        opType = "ne";
    else if (op == "<=")
        opType = "sle";
    else if (op == ">=")
        opType = "sge";
    else if (op == "<")
        opType = "slt";
    else if (op == ">")
        opType = "sgt";
    else
    {
        std::cout << "[emitBooleanBlockRelaEq] this should never happen\n";
        exit(0);
    }
    cb.emit(res->m_reg + " = icmp " + opType + " " + opSize + " " + leftReg + ", " + rightReg);
    int address = cb.emit("br i1 " + res->m_reg + ", @, @");
    res->true_list = cb.makelist({address, FIRST});
    res->false_list = cb.makelist({address, SECOND});
    // res->true_list = cb.merge(res->true_list, lhs->true_list);
    // res->false_list = cb.merge(res->false_list, rhs->false_list);
}
/*
l1: t3 = icmp eq i32 t1, t2
br i1 t3 @, @
*/
void emitBooleanBlockShortCircuit(Node *lhs, Node *rhs, string op, Node *res)
{
    CodeBuffer &cb = CodeBuffer::instance();
    res->m_label = cb.genLabel();
    if (op == "OR")
    {
        cb.bpatch(lhs->false_list, rhs->m_label);
        res->false_list = rhs->false_list;
        res->true_list = cb.merge(lhs->true_list, rhs->true_list);
    }
    else if (op == "AND")
    {
        cb.bpatch(lhs->true_list, rhs->m_label);
        res->true_list = rhs->true_list;
        res->false_list = cb.merge(lhs->false_list, rhs->false_list);
    }
    else
    {
        std::cout << "[emitBooleanBlockShortCircuit] this should never happen\n";
        exit(0);
    }
}
/**
 * (exp1 or exp2)
 * backpatch(ex1.false_list, exp2.label)
 *
 */
#endif
