#include "utils.hpp"
#include "source.hpp"

std::string emitOperator(int val1, int val2, const char *op, const char *type)
{
    std::string returnReg = Allocator::getInstance().fresh_var(op + std::to_string(val1) + "to" + std::to_string(val2));
    CodeBuffer::instance().emit("\t" + returnReg + " = " + op + " " + type + " " + std::to_string(val1) + ", " + std::to_string(val2));
    return returnReg;
}

void emitBinopRes(Node *lhs, Node *rhs, string op, Node *res)
{
    Allocator &allocator = Allocator::getInstance();
    CodeBuffer &codebuffer = CodeBuffer::instance();
    std::string operationSize = "i8";
    if (lhs->m_label == "")
        lhs->m_label = codebuffer.genLabel();
    res->m_label = lhs->m_label;
    std::string leftReg = lhs->getReg();
    std::string rightReg = rhs->getReg();
    std::string resultReg;
    if (lhs->m_type == types::Int || rhs->m_type == types::Int) {
        operationSize = "i32";
        if (lhs->m_type == types::Byte) {
            leftReg = allocator.fresh_var("leftReg8to32");
            codebuffer.emit("\t" + leftReg + " = zext i8 " + lhs->m_reg + " to i32");
        }
        if (rhs->m_type == types::Byte) {
            rightReg = allocator.fresh_var("rightReg8to32");
            codebuffer.emit("\t" + rightReg + " = zext i8 " + rhs->m_reg + " to i32");
        }
    }
    if (op == "/") {
        /**
         * TODO: need to emit a compare of rightReg with 0, and goto somewhere we print division error
         *       we call print in llvm.
         */
        if (operationSize == "i8") {
            resultReg = allocator.fresh_var("udivResult");
            codebuffer.emit("\t" + resultReg + " = udiv " + operationSize + " " + leftReg + ", " + rightReg);
        } else // operationSize == "i32"
        {
            resultReg = allocator.fresh_var("sdivResult");
            codebuffer.emit("\t" + resultReg + " = sdiv " + operationSize + " " + leftReg + ", " + rightReg);
        }
    } else if (op == "*") {
        resultReg = allocator.fresh_var("mulResult");
        codebuffer.emit("\t" + resultReg + " = mul " + operationSize + " " + leftReg + ", " + rightReg);
    } else if (op == "+") {
        resultReg = allocator.fresh_var("addResult");
        codebuffer.emit("\t" + resultReg + " = add " + operationSize + " " + leftReg + ", " + rightReg);
    } else if (op == "-") {
        resultReg = allocator.fresh_var("subResult");
        codebuffer.emit("\t" + resultReg + " = sub " + operationSize + " " + leftReg + ", " + rightReg);
    } else {
        std::cout << "should never reach here!\n";
        exit(0);
    }
    res->m_reg = resultReg;
}

void emitBooleanBlockRelaEq(Node *lhs, Node *rhs, string op, Node *res)
{
    CodeBuffer &cb = CodeBuffer::instance();
    Allocator &allocator = Allocator::getInstance();
    if (lhs->m_label == "")
        lhs->m_label = cb.genLabel();
    res->m_label = lhs->m_label;
    string opType;
    string opSize = "i8";
    string leftReg = lhs->getReg();
    string rightReg = rhs->getReg();
    if (lhs->m_type == types::Int || rhs->m_type == types::Int) {
        opSize = "i32";
        if (lhs->m_type == types::Byte) {
            leftReg = allocator.fresh_var("leftReg8to32");
            cb.emit("\t" + leftReg + " = zext i8 " + lhs->m_reg + " to i32");
        }
        if (rhs->m_type == types::Byte) {
            rightReg = allocator.fresh_var("rightReg8to32");
            cb.emit("\t" + rightReg + " = zext i8 " + rhs->m_reg + " to i32");
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
    else {
        std::cout << "[emitBooleanBlockRelaEq] this should never happen\n";
        exit(0);
    }
    cb.emit("\t" + res->m_reg + " = icmp " + opType + " " + opSize + " " + leftReg + ", " + rightReg);
    int address = cb.emit("\tbr i1 " + res->m_reg + ", label @, label @");
    res->true_list = cb.makelist({ address, FIRST });
    res->false_list = cb.makelist({ address, SECOND });
}

void emitBooleanBlockShortCircuit(Node *lhs, Node *rhs, string op, Node *res)
{
    CodeBuffer &cb = CodeBuffer::instance();
    res->m_label = lhs->m_label;
    if (op == "OR") {
        cb.bpatch(lhs->false_list, rhs->m_label);
        res->false_list = std::move(rhs->false_list);
        res->true_list = cb.merge(lhs->true_list, rhs->true_list);
    } else if (op == "AND") {
        cb.bpatch(lhs->true_list, rhs->m_label);
        res->true_list = std::move(rhs->true_list);
        res->false_list = cb.merge(lhs->false_list, rhs->false_list);
    } else {
        std::cout << "[emitBooleanBlockShortCircuit] this should never happen\n";
        exit(0);
    }
}

void emitLabelAndGoto(Node *res, LabelList &list)
{
    CodeBuffer &cb = CodeBuffer::instance();
    res->m_label = cb.genLabel();
    int address = cb.emit("\tbr label @");
    list.push_back({ address, FIRST });
}

void emitGoto(LabelList &list)
{
    CodeBuffer &cb = CodeBuffer::instance();
    int address = cb.emit("\tbr label @");
    list.push_back({ address, FIRST });
}

std::string openFunctionStack(Node *retType, Node *id, Node *formals)
{
    Allocator &allocator = Allocator::getInstance();
    CodeBuffer &cb = CodeBuffer::instance();
    std::string stack_reg = allocator.fresh_var("stackReg");

    cb.emit("define " + TypeToIRString(retType->m_type) + " @" +
        id->m_name + "(" + TypesToIRString(formals->m_types_list) + ") {");
    cb.emit("\t" + stack_reg + " = alloca i32, i32 50");
    return stack_reg;
}

void storeValInStack(std::string stack_reg, std::string val, unsigned int offset)
{
    CodeBuffer &cb = CodeBuffer::instance();
    Allocator &allocator = Allocator::getInstance();
    std::string ptr = allocator.fresh_var("ptr");
    cb.emit("\t" + ptr + " = getelementptr [50 x i32], [50 x i32]* " + stack_reg + ", i32 0, i32 " + std::to_string(offset));
    cb.emit("\tstore i32 " + val + ", i32* " + ptr);
}

std::string loadValFromStack(std::string stack_reg, unsigned int offset)
{
    CodeBuffer &cb = CodeBuffer::instance();
    Allocator &allocator = Allocator::getInstance();
    std::string ptr = allocator.fresh_var("loadResPtr");
    std::string res = allocator.fresh_var("loadRes");
    cb.emit("\t" + ptr + " = getelementptr [50 x i32], [50 x i32]* " + stack_reg + ", i32 0, i32 " + std::to_string(offset));
    cb.emit("\t" + res + " = load i32, i32* " + ptr);
    return ptr;
}
