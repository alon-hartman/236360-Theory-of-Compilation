#include "source.hpp"
#include <sstream>

void Delete(int count, ...)
{

    va_list list;

    va_start(list, count);

    for (int i = 0; i < count; i++) {
        delete va_arg(list, Node *);
    }

    va_end(list);
}

SymTable::SymTable()
{
    has_main = false;
    Scope scope(0);
    std::vector<types> v1 = { types::String };
    std::vector<types> v2 = { types::Int };
    scope.entries.emplace_back("print", types::Void, v1, true, 0);
    scope.entries.emplace_back("printi", types::Void, v2, true, 0);
    scopes_stack.push_back(scope);
}

SymTable::Scope &SymTable::push(scope_type type)
{
    int offset = this->top().offset;
    scopes_stack.emplace_back(offset);
    this->top().type = type;
    return this->top();
}

void SymTable::pop()
{
    endScope();
    Scope &scope = scopes_stack.back();
    for (auto entry : scope.entries) {
        if (!entry.is_func) {
            printID(entry.name, entry.offset, TypeToString(entry.return_type));
        } else {
            std::vector<std::string> strings_vec = TypesToStrings(entry.types_vec);
            std::string string_type = TypeToString(entry.return_type);
            std::string s = makeFunctionType(string_type, strings_vec);
            printID(entry.name, entry.offset, s);
        }
    }
    scopes_stack.pop_back();
}

SymTable::Scope &SymTable::top()
{
    return scopes_stack.back();
}

void SymTable::insert(Node *node, bool is_func)
{
    if (find_entry(node->m_name) != nullptr) {
        // variable with same name already exists
        errorDef(node->m_lineno, node->m_name);
        exit(0);
    }
    Entry entry = { node->m_name, node->m_type,
                   node->m_types_list,
                   is_func, top().offset };
    if (is_func) {
        this->scopes_stack[0].entries.push_back(entry);
    } else {
        this->top().entries.push_back(entry);
    }
    this->top().offset += !is_func;
    has_main |= (is_func && entry.name == "main");
}

void SymTable::insert_arg(Node *node)
{
    if (find_entry(node->m_name) != nullptr) // TODO: check if needed
    {
        // variable with same name already exists
        errorDef(node->m_lineno, node->m_name);
        exit(0);
    }
    Entry entry = { node->m_name, node->m_type,
                   node->m_types_list, false, top().min_arg_offset-- };
    this->top().entries.push_back(entry);
}

SymTable::Entry *SymTable::find_entry(const std::string &name)
{
    for (int scope_num = scopes_stack.size() - 1; scope_num >= 0; --scope_num) {
        Scope &scope = scopes_stack[scope_num];
        for (int entry_num = 0; entry_num < scope.entries.size(); ++entry_num) {
            Entry &entry = scope.entries[entry_num];
            if (entry.name == name) {
                return &entry;
            }
        }
    }
    return nullptr;
}

bool SymTable::isInScope(SymTable::scope_type scope_type)
{
    for (int scope_num = scopes_stack.size() - 1; scope_num >= 0; --scope_num) {
        Scope &scope = scopes_stack[scope_num];
        if (scope.type == scope_type) {
            return true;
        }
    }
    return false;
}

void check_args_type(Node *node, SymTable::Entry *entry)
{
    if (node->m_types_list.size() != entry->types_vec.size()) {
        std::vector<std::string> strings_vec = TypesToStrings(entry->types_vec);
        errorPrototypeMismatch(node->m_lineno, entry->name, strings_vec);
        exit(0);
    }
    for (int i = 0; i < node->m_types_list.size(); ++i) { // can the passed type be converted to the declared type?
        if (!allowed_implicit_assignment(entry->types_vec[i], node->m_types_list[i])) {
            std::vector<std::string> strings_vec = TypesToStrings(entry->types_vec);
            errorPrototypeMismatch(node->m_lineno, entry->name, strings_vec);
            exit(0);
        }
    }
}

bool allowed_implicit_assignment(types lhs, types rhs)
{
    return (lhs == rhs || (lhs == types::Int && rhs == types::Byte));
}

bool allowed_explicit_assignment(types lhs, Node *node)
{
    if ((lhs != types::Int && lhs != types::Byte) || (node->m_type != types::Byte && node->m_type != types::Int)) {
        return false;
    }
    return true;
}

std::string TypeToString(types type)
{
    switch (type) {
    case types::Bool:
        return "BOOL";
    case types::Byte:
        return "BYTE";
    case types::Int:
        return "INT";
    case types::Void:
        return "VOID";
    case types::String:
        return "STRING";
    default:
        return ":(";
    };
}

std::string TypeToIRString(types type)
{
    switch (type) {
    case types::Bool:
        return "i1";
    case types::Byte:
        return "i8";
    case types::Int:
        return "i32";
    case types::Void:
        return "void";
    default:
        return ":(";
    };
}

std::vector<std::string> TypesToStrings(std::vector<types> &vec)
{
    std::vector<std::string> res;
    for (int i = 0; i < vec.size(); ++i) {
        res.push_back(TypeToString(vec[i]));
    }
    return res;
}

std::string TypesToIRString(std::vector<types> &vec)
{
    std::ostringstream res("");
    for (int i = 0; i < vec.size(); ++i) {
        res << TypeToIRString(vec[i]) << (i == vec.size() - 1 ? "" : ", ");
    }
    return res.str();
}
