#include "source.hpp"
#include "hw3_output.hpp"

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

SymTable::SymTable()
{
    Scope scope(0);
    std::vector<types> v1 = {types::String};
    std::vector<types> v2 = {types::Int};
    scope.entries.emplace_back("print", types::Void, v1, 0);
    scope.entries.emplace_back("printi", types::Void, v2, 0);
}

SymTable::Scope &SymTable::push(scope_type type = scope_type::BLOCK)
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
    for (auto entry : scope.entries)
    {
        if (entry.is_func)
        {
            printID(entry.name, entry.offset, TypeToString(entry.return_type));
        }
        else
        {
            std::string s = makeFunctionType(entry.return_type, TypesToStrings(entry.types_vec));
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
    if (find_entry(node->m_name) != nullptr)
    {
        // variable with same name already exists
        errorDef(node->m_lineno, node->m_name);
        exit(0);
    }
    Entry entry = {node->m_name, node->m_type,
                   node->m_types_list,
                   is_func, top().offset};
    this->top().entries.push_back(entry);
    this->top().offset += is_func;
}

void SymTable::insert_arg(Node *node)
{
    if (find_entry(node->m_name) != nullptr) // TODO: check if needed
    {
        // variable with same name already exists
        errorDef(node->m_lineno, node->m_name);
        exit(0);
    }
    Entry entry = {node->m_name, node->m_type,
                   node->m_types_list, top().min_arg_offset--};
    this->top().entries.push_back(entry);
}

SymTable::Entry *SymTable::find_entry(const std::string &name)
{
    for (int scope_num = 0; scope_num < scopes_stack.size(); ++scope_num)
    {
        Scope &scope = scopes_stack[scope_num];
        for (int entry_num = 0; entry_num < scope.entries.size(); ++entry_num)
        {
            Entry &entry = scope.entries[entry_num];
            if (entry.name == name)
            {
                return &entry;
            }
        }
    }
    return nullptr;
}

void SymTable::check_type(Node *node, SymTable::Entry *entry)
{
    if (entry == nullptr)
    {
        entry = find_entry(node->m_name);
    }
    node->m_types_list;
    entry->types_vec;
    if (node->m_type != entry->return_type)
    {
        errorMismatch(node->m_lineno);
        exit(0);
    }
    assert(node->m_types_list.size() == entry->types_vec.size());
    for (int i = 0; i < node->m_types_list.size(); ++i)
    {
        if (node->m_types_list[i] != entry->types_vec[i])
        {
            errorPrototypeMismatch(node->m_lineno, node->m_name, TypesToString(entry->types_vec));
            exit(0);
        }
    }
}

std::string TypeToString(types type)
{
    switch (type)
    {
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

std::vector<std::string> TypesToStrings(std::vector<types> &vec)
{
    std::vector<std::string> res;
    for (int i = 0; i < vec.size(); ++i)
    {
        res.push_back(TypeToString(vec[i]));
    }
    return res;
}
