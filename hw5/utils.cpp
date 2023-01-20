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
    res->m_label = lhs->m_label;
    std::string calc_label = codebuffer.genLabel();
    std::string leftReg = lhs->m_reg;
    std::string rightReg = rhs->m_reg;
    codebuffer.bpatch(lhs->next_list, rhs->m_label);
    codebuffer.bpatch(rhs->next_list, calc_label);
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
        std::string cmp_reg = allocator.fresh_var("cmpZeroReg");
        std::string next_label = allocator.fresh_var("next_label");
        codebuffer.emit("\t" + cmp_reg + " = icmp eq " + operationSize + " 0" + ", " + rightReg);
        codebuffer.emit("\tbr i1 " + cmp_reg + ", label %div_by_zero_label, label " + next_label);
        codebuffer.emit(next_label.substr(1) + ":");
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
    emitGoto(res->next_list);
}

void emitBooleanBlockRelaEq(Node *lhs, Node *rhs, string op, Node *res)
{
    CodeBuffer &cb = CodeBuffer::instance();
    Allocator &allocator = Allocator::getInstance();
    res->m_label = lhs->m_label;
    string opType;
    string opSize = "i8";
    string leftReg = lhs->m_reg;
    string rightReg = rhs->m_reg;
    string calc_label = cb.genLabel();
    cb.bpatch(lhs->next_list, rhs->m_label);
    cb.bpatch(rhs->next_list, calc_label);
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

void emitBooleanNoOp(Node *id_node)
{
    CodeBuffer &cb = CodeBuffer::instance();
    Allocator &allocator = Allocator::getInstance();
    id_node->m_label = cb.genLabel();
    std::string id_reg = id_node->getReg();
    id_node->m_reg = allocator.fresh_var("cmpReg");
    cb.emit("\t" + id_node->m_reg + " = icmp eq i1 " + id_reg + ", 1");
    int address = cb.emit("\tbr i1 " + id_node->m_reg + ", label @, label @");
    id_node->true_list = cb.makelist({ address, FIRST });
    id_node->false_list = cb.makelist({ address, SECOND });
}

void emitLabelAndGoto(Node *res, LabelList &list)
{
    CodeBuffer &cb = CodeBuffer::instance();
    Allocator &allocator = Allocator::getInstance();
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
    cb.emit("\tbr label %function_start");
    cb.emit("div_by_zero_label:");
    cb.emit("\tcall void @divide_by_zero_error()");
    cb.emit("\tbr label %function_start");
    cb.emit("function_start:");
    cb.emit("\t" + stack_reg + " = alloca i32, i32 50");
    return stack_reg;
}

void storeValInStack(std::string stack_reg, std::string val, unsigned int offset, Node *in)
{
    CodeBuffer &cb = CodeBuffer::instance();
    Allocator &allocator = Allocator::getInstance();
    std::string ptr = allocator.fresh_var("ptr");
    if (in != nullptr) {
        std::string temp_reg = val;
        if (in->m_type == types::Byte) {
            temp_reg = allocator.fresh_var("tempReg8to32");
            cb.emit("\t" + temp_reg + " = zext i8 " + in->m_reg + " to i32");
        }
        if (in->m_type == types::Bool) {
            temp_reg = allocator.fresh_var("tempReg1to32");
            cb.emit("\t" + temp_reg + " = zext i1 " + in->m_reg + " to i32");
        }
        val = temp_reg;
    }
    cb.emit("\t" + ptr + " = getelementptr i32, i32* " + stack_reg + ", i32 " + std::to_string(offset));
    cb.emit("\tstore i32 " + val + ", i32* " + ptr);
}

std::string loadValFromStack(std::string stack_reg, int offset, SymTable::Entry *in)
{
    CodeBuffer &cb = CodeBuffer::instance();
    Allocator &allocator = Allocator::getInstance();
    std::string ptr = allocator.fresh_var("loadResPtr");
    std::string res = allocator.fresh_var("loadRes");
    if (offset < 0) {
        std::string resType = "i32";
        if (in->return_type == types::Byte) {
            resType = "i8";
        }
        if (in->return_type == types::Bool) {
            resType = "i1";
        }
        cb.emit("\t" + res + " = add " + resType + " 0, %" + std::to_string(-1 - offset));
        return res;
    }
    cb.emit("\t" + ptr + " = getelementptr i32, i32* " + stack_reg + ", i32 " + std::to_string(offset));
    cb.emit("\t" + res + " = load i32, i32* " + ptr);
    if (in != nullptr) {
        std::string trunced = allocator.fresh_var("truncedRes");
        if (in->return_type == types::Byte) {
            cb.emit("\t" + trunced + " = trunc i32 " + res + " to i8");
            res = trunced;
        }
        if (in->return_type == types::Bool) {
            cb.emit("\t " + trunced + " = trunc i32 " + res + " to i1");
            res = trunced;
        }
    }
    return res;
}

void emitExpGoto(Node *node)
{
    CodeBuffer &cb = CodeBuffer::instance();
    if (node->m_label == "") {
        // make sure node expression is evaluated and has a label
        node->m_label = cb.genLabel();
        node->m_reg = node->getReg();
    }
    emitGoto(node->next_list);
}

void emitTypeConversion(Node *lhs, Node *rhs)
{
    CodeBuffer &cb = CodeBuffer::instance();
    Allocator &alloc = Allocator::getInstance();
    std::string converted_reg = rhs->getReg();
    if (lhs->m_type == rhs->m_type) {
        return;
    } else if ((lhs->m_type == types::Int) && (rhs->m_type == types::Byte)) {
        cb.emit("\t" + lhs->m_reg + " = zext i8 " + converted_reg + " to i32");
    } else if ((lhs->m_type == types::Byte) && (rhs->m_type == types::Int)) {
        cb.emit("\t" + lhs->m_reg + " = trunc i32 " + converted_reg + " to i8");
    }
}

void emitPrintFunctions()
{
    CodeBuffer &cb = CodeBuffer::instance();
    cb.emitGlobal("declare i32 @printf(i8*, ...)");
    cb.emitGlobal("declare void @exit(i32)");
    cb.emitGlobal("@.int_specifier = constant [4 x i8] c\"%d\\0A\\00\"");
    cb.emitGlobal("@.str_specifier = constant [4 x i8] c\"%s\\0A\\00\"");
    cb.emitGlobal("");
    cb.emitGlobal("define void @printi(i32) {");
    cb.emitGlobal("    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0");
    cb.emitGlobal("    call i32 (i8*, ...) @printf(i8* %spec_ptr, i32 %0)");
    cb.emitGlobal("    ret void");
    cb.emitGlobal("}");
    cb.emitGlobal("");
    cb.emitGlobal("define void @print(i8*) {");
    cb.emitGlobal("    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0");
    cb.emitGlobal("    call i32 (i8*, ...) @printf(i8* %spec_ptr, i8* %0)");
    cb.emitGlobal("    ret void");
    cb.emitGlobal("}");
    cb.emitGlobal("@divide_by_zero_message = constant [23 x i8] c\"Error division by zero\\00\"");
    cb.emitGlobal("define void @divide_by_zero_error() {");
    cb.emitGlobal("    %divide_by_zero_ptr = getelementptr [23 x i8], [23 x i8]* @divide_by_zero_message, i32 0, i32 0");
    cb.emitGlobal("    call void @print(i8* %divide_by_zero_ptr)");
    cb.emitGlobal("    call void @exit(i32 0)");
    cb.emitGlobal("    ret void");
    cb.emitGlobal("}");
}


std::string emitStringDeclarationAndAssignment(Node *node)
{
    std::string content = node->m_name.substr(1, node->m_name.length() - 2);
    Allocator &allocator = Allocator::getInstance();
    CodeBuffer &cb = CodeBuffer::instance();
    std::string declaration_reg = allocator.fresh_var("stringDecVar");
    declaration_reg.replace(declaration_reg.begin(), declaration_reg.begin() + 1, "@");  // replace % with @
    std::string res = allocator.fresh_var("stringVar");
    std::string content_length = std::to_string(content.length() + 1);  // + 1 for '\0'

    cb.emitGlobal(declaration_reg + " = constant [" + content_length + " x i8] c\"" + content + "\\00\"");

    node->m_label = cb.genLabel();
    cb.emit("\t" + res + " = getelementptr [" + content_length + " x i8], [" + content_length + " x i8]* " + declaration_reg + ", i32 0, i32 0");
    emitGoto(node->next_list);
    return res;
}

void emitFunctionCall(Node *func, SymTable::Entry *entry)
{
    CodeBuffer &cb = CodeBuffer::instance();
    Allocator &allocator = Allocator::getInstance();
    std::string parameters_string;
    assert(entry->types_vec == func->m_types_list);
    for (int i = 0; i < func->m_types_list.size(); ++i) {
        parameters_string += TypeToIRString(func->m_types_list[i]) + " ";
        parameters_string += func->m_reg_list[i] + ", ";
    }
    parameters_string = parameters_string.substr(0, parameters_string.size() - 2);
    func->m_label = cb.genLabel();
    if (entry->return_type == types::Void) {
        cb.emit("\tcall " + TypeToIRString(entry->return_type) + " @" + entry->name + "(" + parameters_string + ")");
    } else {
        func->m_reg = allocator.fresh_var("callRes");
        cb.emit("\t" + func->m_reg + " = call " + TypeToIRString(entry->return_type) + " @" + entry->name + "(" + parameters_string + ")");
    }
    emitGoto(func->next_list);
}

void emitFunctionReturn(Node *node, Node *return_val)
{
    CodeBuffer &cb = CodeBuffer::instance();
    node->m_label = cb.genLabel();
    if (return_val != nullptr) {
        std::string return_reg = return_val->m_reg;
        if (return_reg == "" && return_val->m_type != types::Void)
            return_reg = "0";
        cb.emit("\tret " + TypeToIRString(return_val->m_type) + " " + return_reg);
    } else {
        cb.emit("\tret void");
    }
}

void debug()
{
    return;
}

/**
 * TODO: IN FRIDAY -
 * when reading TRUE or FALSE, we need to create a register of this node with val 0/1,
 * inside a basic block - label and goto. because current implementation (no assignemnt to register, just wire
 * truelist/falselist) doesnt work because that it doesnt bpatch previous basic block's goto statment.
 */

