#include "allocator.h"

using namespace std;

Allocator::Allocator() : reg_num(0) {}

Allocator &Allocator::getInstance()
{
    static Allocator instance;
    return instance;
}

std::string Allocator::fresh_var(const string &name)
{
    return "%" + name + "_" + to_string(reg_num++);
}
